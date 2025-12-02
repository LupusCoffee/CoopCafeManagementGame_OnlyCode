// Fill out your copyright notice in the Description page of Project Settings.

#include "Customer.h"

void ACustomer::BeginPlay()
{
	Super::BeginPlay();
}

ABasicAIController* ACustomer::GetBasicAIController()
{
	return AIController;
}

void ACustomer::SetBasicAIController(ABasicAIController* _controller)
{
	if (IsValid(_controller))
		AIController = _controller;
}

void ACustomer::Hover_Implementation(FInteractionContext Context)
{
	IInteractable::Hover_Implementation(Context);
}

void ACustomer::Unhover_Implementation(FInteractionContext Context)
{
	IInteractable::Unhover_Implementation(Context);
}

bool ACustomer::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	if (IsValid(AIController))
	{
		AIController->TakeOder();
		return true;
	}	
	
	return IInteractable::InteractStarted_Implementation(ActionId, Context);
}

ABasicAIController* ACustomer::GetAIController()
{
	return AIController;
}

void ACustomer::Enable()
{
	Enabled = true;

	OnUpdateCustomerActiveStatus.Broadcast(this);
}

void ACustomer::Disable()
{
	Enabled = false;

	OnUpdateCustomerActiveStatus.Broadcast(this);
}

bool ACustomer::IsEnabled()
{
	return Enabled;
}
