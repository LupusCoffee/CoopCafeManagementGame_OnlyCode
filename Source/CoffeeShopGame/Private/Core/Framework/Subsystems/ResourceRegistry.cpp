// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Framework/Subsystems/ResourceRegistry.h"
#include "Core/Framework/PlayerStates/CoffeeShopPlayerState.h"

void UResourceRegistry::AddToBalance(float aAmount)
{
	AActor* owner = LocalPlayerState->GetOwner();
	LocalPlayerState->SyncToServer(aAmount, ESyncAction::Add);
}

void UResourceRegistry::AddToBalanceInternal(float aAmount)
{
	Balance += aAmount;

	OnBalanceChangedDelegate.Broadcast(Balance);
}

float UResourceRegistry::GetBalance()
{
	return Balance;
}

void UResourceRegistry::SetBalance(float NewBalance)
{
	LocalPlayerState->SyncToServer(NewBalance, ESyncAction::Set);
}

void UResourceRegistry::SetBalanceInternal(float NewBalance)
{
	Balance = NewBalance;

	OnBalanceChangedDelegate.Broadcast(Balance);
}

void UResourceRegistry::SetPlayerState(ACoffeeShopPlayerState* PlayerState)
{
	LocalPlayerState = PlayerState;
}
