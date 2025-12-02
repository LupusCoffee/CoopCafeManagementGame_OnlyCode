#pragma once

#include "CoreMinimal.h"
#include "ActionId.generated.h"

UENUM(BlueprintType)
enum class EActionId : uint8
{
	E,
	Q,
	LeftMouseButton,
	RightMouseButton,
	None
};
