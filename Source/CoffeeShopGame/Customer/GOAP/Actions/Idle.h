// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Customer/GOAP/GOAP_Action.h"
#include "Idle.generated.h"

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UIdle : public UGOAP_Action
{
public:
	virtual bool CheckProceduralPrecondition(FWorldState worldState) override;
	virtual void ApplyMutatedEffect(FWorldState& worldState) override;
private:
	GENERATED_BODY()

public:
	UIdle();
};
