// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontOfQueuePoint.h"

#include "CoffeeShopGame/Managers/QueueManager.h"
#include "CoffeeShopGame/Managers/UCustomerAISubsystem.h"


// Sets default values
AFrontOfQueuePoint::AFrontOfQueuePoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AFrontOfQueuePoint::BeginPlay()
{
	Super::BeginPlay();

	if (UCustomerAISubsystem* CustomerAISubsystem = GetWorld()->GetSubsystem<UCustomerAISubsystem>())
	{
		//CustomerAISubsystem->GetQueueManager()->frontOfQueuePosition = this;
	}
}

