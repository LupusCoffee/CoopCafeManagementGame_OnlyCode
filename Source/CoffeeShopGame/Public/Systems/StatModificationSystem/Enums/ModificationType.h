#pragma once

#include "CoreMinimal.h"
#include "ModificationType.generated.h"

UENUM(BlueprintType)
enum class EModificationType : uint8
{
	Multiplication,
	Division,
	Addition,
	Subtraction
};
