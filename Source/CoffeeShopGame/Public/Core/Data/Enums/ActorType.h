#pragma once

#include "CoreMinimal.h"
#include "ActorType.generated.h"

UENUM(BlueprintType)
enum class EActorType : uint8
{
	Item		UMETA(DisplayName="Item"),
	Machine		UMETA(DisplayName="Machine"),
	Customer	UMETA(DisplayName="Customer")
};
