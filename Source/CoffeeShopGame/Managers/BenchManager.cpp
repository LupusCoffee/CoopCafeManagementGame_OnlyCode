// Fill out your copyright notice in the Description page of Project Settings.

#include "BenchManager.h"

#include "CoffeeShopGame/Customer/Customer.h"
#include "Kismet/GameplayStatics.h"

ABenchManager::ABenchManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Ensure this manager can be found by actor tag queries.
	Tags.AddUnique(FName("BenchManager"));
}

void ABenchManager::BeginPlay()
{
	Super::BeginPlay();

	// Find all actors with the "Bench" tag
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Bench"), FoundActors);

	Benches = FoundActors;

	if (Benches.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("BenchManager: Found %d benches"), Benches.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BenchManager: No benches found with tag 'Bench'"));
	}
}

AActor* ABenchManager::GetClosestBench(AActor* TargetActor) const
{
	if (!TargetActor || Benches.Num() == 0)
	{
		return nullptr;
	}

	AActor* ClosestBench = nullptr;
	float ClosestDistance = FLT_MAX;

	for (AActor* Bench : Benches)
	{
		if (Bench && Bench != TargetActor)
		{
			float Distance = FVector::Dist(TargetActor->GetActorLocation(), Bench->GetActorLocation());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestBench = Bench;
			}
		}
	}

	return ClosestBench;
}

void ABenchManager::Interact_Implementation(AActor* Agent)
{
	if (auto customer = Cast<ACustomer>(Agent))
	{
		customer->SetTargetActor(GetClosestBench(Agent));
		
		customer->MoveToTargetActorAndInteract(true);
	}
	
	IGOAPInteractable::Interact_Implementation(Agent);
}
