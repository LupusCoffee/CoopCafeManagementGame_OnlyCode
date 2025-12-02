// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAP_WaitForOrderAction.h"

bool UGOAP_WaitForOrderAction::CheckProceduralPrecondition(FWorldState worldState)
{
	if (worldState.HasOrdered	== Preconditions.HasOrdered &&
		worldState.IsAtStore	== Preconditions.IsAtStore &&
		worldState.HasCoffee	== Preconditions.HasCoffee)
		return true;

	return false;
}

void UGOAP_WaitForOrderAction::ApplyMutatedEffect(FWorldState& worldState)
{
	worldState.HasCoffee = Effects.HasCoffee;
}

UGOAP_WaitForOrderAction::UGOAP_WaitForOrderAction()
{
	ActionEnum = EGOAPActions::WaitForOrder;
	Cost = 1;
	
	Preconditions.HasOrdered = true;
	Preconditions.IsAtStore = true;
	Preconditions.HasCoffee = false;

	Effects.HasCoffee = true;
}
