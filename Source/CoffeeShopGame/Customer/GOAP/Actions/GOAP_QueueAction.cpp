// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAP_QueueAction.h"

bool UGOAP_QueueAction::CheckProceduralPrecondition(FWorldState worldState)
{
	if (worldState.HasOrdered	== Preconditions.HasOrdered &&
		worldState.IsAtStore	== Preconditions.IsAtStore)
		return true;

	return false;
}

void UGOAP_QueueAction::ApplyMutatedEffect(FWorldState& worldState)
{
	worldState.HasOrdered = Effects.HasOrdered;
}

UGOAP_QueueAction::UGOAP_QueueAction()
{
	ActionEnum = EGOAPActions::Queue;
	Cost = 1;
	
	Preconditions.HasOrdered = false;
	Preconditions.IsAtStore = true;

	Effects.HasOrdered = true;
}
