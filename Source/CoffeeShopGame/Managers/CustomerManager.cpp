// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomerManager.h"

#include "UCustomerAISubsystem.h"


// Sets default values
ACustomerManager::ACustomerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACustomerManager::BeginPlay()
{
	Super::BeginPlay();

	if (UCustomerAISubsystem* CustomerAISubsystem = GetWorld()->GetSubsystem<UCustomerAISubsystem>())
	{
		CustomerAISubsystem->AssignCustomerManager(this);
	}
}

void ACustomerManager::SpawnCustomerServer_Implementation(FVector SpawnLocation)
{
	if (HasAuthority())
		SpawnCustomer(SpawnLocation);
}

ACustomer* ACustomerManager::SpawnCustomer(FVector SpawnLocation)
{
	FActorSpawnParameters SpawnParams;
	auto Spawned = GetWorld()->SpawnActor<ACustomer>(
		ActorToSpawn,
		SpawnLocation,
		GetActorRotation(),
		SpawnParams
	);

	if (IsValid(Spawned))
	{
		Spawned->OnUpdateCustomerActiveStatus.AddDynamic(this, &ACustomerManager::UpdateCustomerStatus);
		AddCustomer(Spawned);
		
		return Spawned;
	}

	

	return nullptr;
}

void ACustomerManager::ActivateCustomers(int n)
{
	if (!HasAuthority()) return;
	
	if (n <= 0 || InactiveCustomers[0] == nullptr || InactiveCustomers.IsEmpty())
		return;
	
	InactiveCustomers[0]->Enable();

	ActivateCustomers(n-1);
}

bool ACustomerManager::AddCustomer(ACustomer* customer)
{
	if (!IsValid(customer)) return false;
	
	AllCustomers.Add(customer);

	if (customer->IsEnabled())
		ActiveCustomers.Add(customer);
	else
		InactiveCustomers.Add(customer);
	
	return true;
}

TArray<ACustomer*> ACustomerManager::GetAllCustomers()
{
	return AllCustomers;
}

void ACustomerManager::UpdateCustomerStatus(ACustomer* customer)
{
	if (!IsValid(customer)) return;

	if (customer->IsEnabled())
	{
		if (InactiveCustomers.Contains(customer))
			InactiveCustomers.Remove(customer);
		if (!ActiveCustomers.Contains(customer))
			ActiveCustomers.Add(customer);
	}
	else
	{
		if (ActiveCustomers.Contains(customer))
			ActiveCustomers.Remove(customer);
		if (!InactiveCustomers.Contains(customer))
			InactiveCustomers.Add(customer);
	}
}

void ACustomerManager::ForceCustomersThirsty(int customersNumber)
{
	if (customersNumber >= ActiveCustomers.Num())
		customersNumber = ActiveCustomers.Num()-1;

	int i = 0;
	int n = 0;
	while (i != customersNumber)
	{
		if (ActiveCustomers[n]->GetAIController()->GOAP->CurrentWorldState.Thirstiness >= 1.0f)
		{
			n++;
			if (n >= ActiveCustomers.Num())
				break;
			continue;
		}
		
		ActiveCustomers[n]->GetAIController()->GOAP->CurrentWorldState.Thirstiness = 1.0f;
		ActiveCustomers[n]->GetAIController()->GOAP->EvaluateGoals();

		i++;
		n++;
	}
}
