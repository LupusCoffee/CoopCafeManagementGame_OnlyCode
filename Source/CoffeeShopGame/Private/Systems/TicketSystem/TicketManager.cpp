// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/TicketSystem/TicketManager.h"

#include "CoffeeShopGame/Managers/LoopManagementSubsystem.h"

class ULoopManagementSubsystem;
// Sets default values
ATicketManager::ATicketManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATicketManager::BeginPlay()
{
	Super::BeginPlay();
	if (auto* LoopManager = GetWorld()->GetSubsystem<ULoopManagementSubsystem>())
	{
		LoopManager->SetTicketManagerReference(this);
	}
		
}

// Called every frame
void ATicketManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

