// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAP_FIndStoreAction.h"

bool UGOAP_FIndStoreAction::CheckProceduralPrecondition(FWorldState worldState)
{
	if (worldState.IsAtStore == Preconditions.IsAtStore)
		return true;

	return false;
}

void UGOAP_FIndStoreAction::ApplyMutatedEffect(FWorldState& worldState)
{
	worldState.IsAtStore = Effects.IsAtStore;
}

UGOAP_FIndStoreAction::UGOAP_FIndStoreAction()
{
	ActionEnum = EGOAPActions::FindStore;
	Cost = 1;
	
	Preconditions.IsAtStore = false;

	Effects.IsAtStore = true;
}
