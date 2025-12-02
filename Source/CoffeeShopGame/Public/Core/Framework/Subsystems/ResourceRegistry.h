// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ResourceRegistry.generated.h"

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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetBalance();

public:
	UPROPERTY(BlueprintAssignable, EditAnywhere)
	FOnBalanceChangedSignature OnBalanceChangedDelegate;

private:
	float Balance;
	
};
