// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Customer/GOAP/GOAP_Goal.h"
#include "DrinkCoffee.generated.h"

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UDrinkCoffee : public UGOAP_Goal
{
public:
	virtual bool IsValid(FWorldState CurrentState) const override;

	UDrinkCoffee();
private:
	GENERATED_BODY()
};
