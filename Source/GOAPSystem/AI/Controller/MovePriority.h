#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMovePriority : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	LowPriority	UMETA(DisplayName = "Low Priority"),
	Normal		UMETA(DisplayName = "Normal"),
	HighPriority	UMETA(DisplayName = "High Priority"),
};

