// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Framework/Subsystems/ResourceRegistry.h"

void UResourceRegistry::AddToBalance(float aAmount)
{
	Balance += aAmount;

	OnBalanceChangedDelegate.Broadcast(Balance);
}

float UResourceRegistry::GetBalance()
{
	return Balance;
}
