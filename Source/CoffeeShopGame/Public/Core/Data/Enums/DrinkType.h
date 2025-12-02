#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDrinkType : uint8
{
	BlackCoffee UMETA(DisplayName = "BlackCoffee"),
	Latte UMETA(DisplayName = "Latte"),
	Cappuccino UMETA(DisplayName = "Cappuccino"),
	SpiderBeanCoffee UMETA(DisplayName = "SpiderBeanCoffee")
};