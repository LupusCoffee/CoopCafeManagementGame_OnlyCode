// Fill out your copyright notice in the Description page of Project Settings.


#include "DrinkCoffee.h"

bool UDrinkCoffee::IsValid(FWorldState CurrentState) const
{
	if (DesiredState.Thirstiness >= CurrentState.Thirstiness)
		return true;

	return false;
}

UDrinkCoffee::UDrinkCoffee()
{
	GoalName = "DrinkCoffee";
	DesiredState.Thirstiness = 0.0f;
}
