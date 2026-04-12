// Fill out your copyright notice in the Description page of Project Settings.


#include "RoamingManager.h"
#include "../Customer/Misq/AIRoamArea.h"
#include "../Customer/Customer.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"


void ARoamingManager::Interact_Implementation(AActor* Agent)
{
	if (!HasAuthority())
	{
		if (ACustomer* Customer = Cast<ACustomer>(Agent))
		{
			RequestAIRoam(Customer, 20.f);
		}
		return;
	}

	RequestAIRoam(Cast<ACustomer>(Agent), 20.f);
	IGOAPInteractable::Interact_Implementation(Agent);
}

// Sets default values
ARoamingManager::ARoamingManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ARoamingManager::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	RoamAreas.Reset();
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AAIRoamArea> It(World); It; ++It)
		{
			AddRoamArea(*It);
		}
	}
}

void ARoamingManager::AddRoamArea(AAIRoamArea* RoamArea)
{
	if (!HasAuthority() || !IsValid(RoamArea) || RoamAreas.Contains(RoamArea))
	{
		return;
	}

	RoamAreas.Add(RoamArea);
}

void ARoamingManager::RequestAIRoam(ACustomer* Agent, float RoamingTime)
{
	if (!HasAuthority())
	{
		Server_RequestAIRoam(Agent, RoamingTime);
		return;
	}

	if (!IsValid(Agent) || RoamAreas.Num() == 0)
	{
		return;
	}

	float distanceToClosest = TNumericLimits<float>::Max();
	AAIRoamArea* closestRoamArea = nullptr;
	
	for (AAIRoamArea* Element : RoamAreas)
	{
		if (IsValid(Element))
		{
			float distance = FVector::Dist(Element->GetActorLocation(), Agent->GetActorLocation());
			if (distance < distanceToClosest)
			{
				distanceToClosest = distance;
				closestRoamArea = Element;
			}
		}
	}
	
	if (IsValid(closestRoamArea))
	{
		closestRoamArea->RequestAIRoam(Agent, RoamingTime);
	}
}

void ARoamingManager::Server_RequestAIRoam_Implementation(ACustomer* Agent, float RoamingTime)
{
	RequestAIRoam(Agent, RoamingTime);
}

void ARoamingManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARoamingManager, RoamAreas);
}
