// Fill out your copyright notice in the Description page of Project Settings.


#include "QueueManager.h"

#include "GameFramework/Character.h"

AQueueManager::AQueueManager(): FrontOfQueuePosition(nullptr)
{
}

void AQueueManager::BeginPlay()
{
	Super::BeginPlay();
	
	QueuePositions.Add(GetActorLocation());
	GenerateQueuePositions(8);
}

void AQueueManager::GenerateQueuePositions(int _amountOfPositionsToGenerate)
{
	auto headPosition = FrontOfQueuePosition->GetActorLocation();
	auto headForwardVector = FrontOfQueuePosition->GetActorForwardVector();

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

bool AQueueManager::CanAddCustomer()
{
	return CustomerList.Num() < QueuePositions.Num();
}

void AQueueManager::CustomerLeavesQueue(ACharacter* customer)
{
	CustomerList.RemoveAt(CustomerList.IndexOfByKey(customer));
}

void AQueueManager::AddCustomer(ACharacter* customer, FVector &outLocationToMoveTo)
{
	if (!HasAuthority())
		return;

	CustomerList.Add(customer);
	
	outLocationToMoveTo = QueuePositions[CustomerList.IndexOfByKey(customer)];
}

void AQueueManager::GetQueuePositionOfCustomer(ACharacter* customer, FVector& outLocationToMoveTo)
{
	if (CustomerList.IndexOfByKey(customer) < 0)
		return;
	
	outLocationToMoveTo = QueuePositions[CustomerList.IndexOfByKey(customer)];
}

void AQueueManager::RelocateAllCustomers()
{
	if (!HasAuthority())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Removing customer"));
	
	for (int i = 0; i < CustomerList.Num(); i++)
	{
		//CustomerList[i]->SendRelocateCustomer();
	}
}

ACharacter* AQueueManager::GetFirstInQueue()
{
	if (CustomerList.Num() != 0)
		return CustomerList[0];
	
	return nullptr;

	
}
