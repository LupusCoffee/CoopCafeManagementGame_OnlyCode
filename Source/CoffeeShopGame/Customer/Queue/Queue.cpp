// Fill out your copyright notice in the Description page of Project Settings.


#include "Queue.h"

#include "CoffeeShopGame/Customer/Controllers/BasicAIController.h"


// Sets default values
AQueue::AQueue()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ChairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairMesh"));
	SetRootComponent(ChairMesh);
}

// Called when the game starts or when spawned
void AQueue::BeginPlay()
{
	Super::BeginPlay();

	GenerateQueuePositions(8);
}

// Called every frame
void AQueue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AQueue::AddCustomer(APawn* customer)
{
	if (!IsValid(customer))
		return false;

	if (!CanAddCustomer()) return false;

	if (ABasicAIController* AIController = Cast<ABasicAIController>(customer->GetController()))
	{
		CustomerList.Add(customer);
		AIController->AddQueue(this);
		AIController->MoveToLocation(QueuePositions[CustomerList.IndexOfByKey(customer)], 5.0f);
		return true;
	}
	return false;
}

void AQueue::RelocateAllCustomers()
{
	for (auto customer : CustomerList)
	{
		if (!IsValid(customer)) continue;
		
		if (ABasicAIController* AIController = Cast<ABasicAIController>(customer->GetController()))
		{
			AIController->MoveToLocation(QueuePositions[CustomerList.IndexOfByKey(customer)], 5.0f);
		}
	}
}

bool AQueue::CanAddCustomer()
{
	return CustomerList.Num() < QueuePositions.Num();
}

void AQueue::GenerateQueuePositions(int _amountOfPositionsToGenerate)
{
	auto headPosition = GetActorLocation();
	auto headForwardVector = GetActorForwardVector();

	for (int i = 1; i < _amountOfPositionsToGenerate; i++)
	{
		QueuePositions.Add(headPosition - headForwardVector * (i * DistanceBetweenQueuePositions));
	}

	for (auto position : QueuePositions)
	{
		DrawDebugCircle(
		   GetWorld(),
		   position,
		   10,
		   32,
		   FColor::Green,
		   true,
		   2,
		   0,
		   10.0f,     // Thickness
		   FVector(1, 0, 0),  // Axis X
		   FVector(0, 1, 0),  // Axis Y
		   true               // Draw full circle (not arc)
	   );
	}
	
}

void AQueue::CustomerLeavesQueue(APawn* customer)
{
	if (ABasicAIController* AIController = Cast<ABasicAIController>(customer->GetController()))
	{
		AIController->RemoveQueue();
	}
	CustomerList.RemoveAt(CustomerList.IndexOfByKey(customer));
	RelocateAllCustomers();
}

