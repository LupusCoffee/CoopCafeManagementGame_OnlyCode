// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAP_Enums.generated.h"

UENUM(BlueprintType)
enum class EGOAPActions : uint8 {
	None	= 0	UMETA(DisplayName = "None"),
	Idle	= 1	UMETA(DisplayName = "Idle"),
	FindStore = 2 UMETA(DisplayName = "FindStore"),
	Queue 	= 3	UMETA(DisplayName = "Queue"),
	WaitForOrder = 4	UMETA(DisplayName = "WaitForOrder"),
	DrinkCoffee = 5	UMETA(DisplayName = "DrinkCoffee")
};