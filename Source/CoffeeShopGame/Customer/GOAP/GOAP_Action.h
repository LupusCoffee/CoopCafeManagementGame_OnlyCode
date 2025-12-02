// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAP_Component.h"
#include "UObject/Object.h"
#include "GOAP_Action.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class COFFEESHOPGAME_API UGOAP_Action : public UObject
{
	GENERATED_BODY()

	public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGOAPActions ActionEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldState Preconditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldState Effects;

	UFUNCTION(BlueprintCallable)
	virtual bool CheckProceduralPrecondition(FWorldState worldState) { return true; }

	UFUNCTION(BlueprintCallable)
	virtual void ApplyMutatedEffect(FWorldState& worldState) { };
};
