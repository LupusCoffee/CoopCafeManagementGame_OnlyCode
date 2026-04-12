// Fill out your copyright notice in the Description page of Project Settings.


#include "MoneyObject.h"
#include "Core/Framework/Subsystems/ResourceRegistry.h"

void AMoneyObject::Local_StartHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_StartHover_Implementation(Context);
}

bool AMoneyObject::Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	return IInteractable::Server_EndInteraction_Implementation(ActionId, Context);
}

bool AMoneyObject::Server_TickInteraction_Implementation(EActionId ActionId, FPlayerContext Context, float DeltaTime)
{
	return IInteractable::Server_TickInteraction_Implementation(ActionId, Context, DeltaTime);
}

bool AMoneyObject::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	GetWorld()->GetGameInstance()->GetSubsystem<UResourceRegistry>()->AddToBalance(Amount);

	Destroy();
	
	return IInteractable::Server_StartInteraction_Implementation(ActionId, Context);
	
	
}

void AMoneyObject::Local_EndHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_EndHover_Implementation(Context);
}

// Sets default values
AMoneyObject::AMoneyObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMoneyObject::SetAmount(float InAmount)
{
	Amount = InAmount;
}

float AMoneyObject::GetAmount()
{
	return Amount;
}


// Called when the game starts or when spawned
void AMoneyObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMoneyObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

