// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ResourceRegistry.generated.h"

class ACoffeeShopPlayerState;

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UResourceRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBalanceChangedSignature, float, NewBalance);

public:
	UFUNCTION(BlueprintCallable)
	void AddToBalance(float aAmount);
	void AddToBalanceInternal(float aAmount);

	UFUNCTION(BlueprintCallable)
	void SetBalance(float NewBalance);
	void SetBalanceInternal(float NewBalance);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetBalance();

	void SetPlayerState(ACoffeeShopPlayerState* PlayerState);

public:
	UPROPERTY(BlueprintAssignable, EditAnywhere)
	FOnBalanceChangedSignature OnBalanceChangedDelegate;

private:
	float Balance;

	UPROPERTY();
	ACoffeeShopPlayerState* LocalPlayerState;
};
