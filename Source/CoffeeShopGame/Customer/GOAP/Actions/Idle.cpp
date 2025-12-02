// Fill out your copyright notice in the Description page of Project Settings.


#include "Idle.h"

bool UIdle::CheckProceduralPrecondition(FWorldState worldState)
{
	if (worldState.Thirstiness <= Preconditions.Thirstiness)
		return true;

	return false;
}

void UIdle::ApplyMutatedEffect(FWorldState& worldState)
{
	worldState.Thirstiness = Effects.Thirstiness;
}

UIdle::UIdle()
{
	ActionEnum = EGOAPActions::Idle;
	Cost = 1;

	Preconditions.Thirstiness = 0.75f;

	Effects.Thirstiness = 1.0f;
}
