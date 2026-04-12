// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Framework/PlayerStates/CoffeeShopPlayerState.h"
#include "Core/Framework/Subsystems/ResourceRegistry.h"

void ACoffeeShopPlayerState::BeginPlay()
{
	ResourceRegistry = GetWorld()->GetGameInstance()->GetSubsystem<UResourceRegistry>();

	if (ResourceRegistry)
		ResourceRegistry->SetPlayerState(this);
}

void ACoffeeShopPlayerState::SyncToServer_Implementation(float Amount, ESyncAction SyncAction)
{
	switch (SyncAction)
	{
	case ESyncAction::Set:
		ResourceRegistry->SetBalanceInternal(Amount);
		break;
	case ESyncAction::Add:
		ResourceRegistry->AddToBalanceInternal(Amount);
		break;
	}

	SyncToClient(ResourceRegistry->GetBalance());
}

void ACoffeeShopPlayerState::SyncToClient_Implementation(float Amount)
{
	ResourceRegistry->SetBalanceInternal(Amount);
}