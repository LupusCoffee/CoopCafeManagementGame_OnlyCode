// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PointOfInterestType.generated.h"

UENUM(BlueprintType)
enum class EPointOfInterestType : uint8 {
	NormalArea	= 0	UMETA(DisplayName = "NormalArea"),
	Store = 1		UMETA(DisplayName = "Store"),
	
};