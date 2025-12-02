// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAP_DrinkCoffeeAction.h"

bool UGOAP_DrinkCoffeeAction::CheckProceduralPrecondition(FWorldState worldState)
{
	if (worldState.HasCoffee == Preconditions.HasCoffee)
		return true;

	return false;
}

void UGOAP_DrinkCoffeeAction::ApplyMutatedEffect(FWorldState& worldState)
{
	worldState.Thirstiness = Effects.Thirstiness;
	worldState.HasCoffee = Effects.HasCoffee;
	worldState.HasOrdered = Effects.HasOrdered;
}

UGOAP_DrinkCoffeeAction::UGOAP_DrinkCoffeeAction()
{
	ActionEnum = EGOAPActions::DrinkCoffee;
	Cost = 1;
	
	Preconditions.HasCoffee = true;
	
	Effects.Thirstiness = 0.0f;;
	Effects.HasCoffee = false;
	Effects.HasOrdered = false;
}
