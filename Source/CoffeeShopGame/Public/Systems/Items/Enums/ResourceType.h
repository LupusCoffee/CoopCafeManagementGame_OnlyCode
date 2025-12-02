#pragma once

#include "CoreMinimal.h"
#include "ResourceType.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None					UMETA(Hidden),
	

	
	//BASE INGREDIENTS
	BaseIngredientsHeader	UMETA(DisplayName="=== BASE INGREDIENTS ============"),
	
	CoffeeBeans				UMETA(DisplayName="Beans   | CoffeeBeans"),
	SpiderBeans				UMETA(DisplayName="Beans   | SpiderBeans"),
	CoffeePowder			UMETA(DisplayName="Powder | CoffeePowder"),
	SpiderPowder			UMETA(DisplayName="Powder | SpiderPowder"),
	
	Space1					UMETA(DisplayName="                                 "),

	
	//BASE DRINKS
	BaseDrinksHeader		UMETA(DisplayName="=== BASE DRINKS ================="),
	
	BasicCoffee				UMETA(DisplayName="BasicCoffee"),
	SpiderCoffee			UMETA(DisplayName="SpiderCoffee"),
	Milk					UMETA(DisplayName="Milk"),
	
	Space2					UMETA(DisplayName="                                 "),

	
	//DRINKS
	DrinksHeader			UMETA(DisplayName="=== DRINKS ======================="),
	
	BasicCoffeeWithMilk		UMETA(DisplayName="BasicCoffeeWithMilk"),
	SpiderCoffeeWithMilk	UMETA(DisplayName="SpiderCoffeeWithMilk"),
	Latte					UMETA(DisplayName="Latte"),
	
	Space3					UMETA(DisplayName="                                 "),

	
	//MISC
	MiscHeader				UMETA(DisplayName="=== MISC ========================="),
	
	Energy					UMETA(DisplayName="Energy")
};
