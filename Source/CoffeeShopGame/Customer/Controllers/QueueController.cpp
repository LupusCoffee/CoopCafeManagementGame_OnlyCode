// Fill out your copyright notice in the Description page of Project Settings.


#include "QueueController.h"

#include "CoffeeShopGame/Customer/Customer.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
AQueueController::AQueueController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

ACustomer* AQueueController::GetCustomer() const
{
	if (IsValid(Customer))
	return Customer;
	
	return nullptr;
}

void AQueueController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.IsSuccess())
	{
		MoveCompleted.Broadcast(Customer);
	}
	
	Super::OnMoveCompleted(RequestID, Result);
}

void AQueueController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (auto* cust = Cast<ACustomer>(InPawn))
	{
		cust->QueueController = this;
		Customer = cust;
	}
}

AQueue* AQueueController::GetQueue()
{
	return CurrentQueue.Get();
}

void AQueueController::SetQueue(AQueue* queue)
{
	if (IsValid(queue))
		CurrentQueue = queue;
}

bool AQueueController::IsInQueue()
{
	if (!GetQueue())
		return false;
	
	return CurrentQueue->GetPositionInQueue(GetCustomer()) != INDEX_NONE;
}
