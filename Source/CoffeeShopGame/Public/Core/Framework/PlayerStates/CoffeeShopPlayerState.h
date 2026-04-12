// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CoffeeShopPlayerState.generated.h"

class UResourceRegistry;

UENUM()
enum class ESyncAction : uint8
{
	Set,
	Add
};

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API ACoffeeShopPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(Remote, Reliable)
	void SyncToClient(float Amount);

	UFUNCTION(Server, Reliable)
	void SyncToServer(float Amount, ESyncAction SyncAction);

private:
	UPROPERTY();
	UResourceRegistry* ResourceRegistry;
};
