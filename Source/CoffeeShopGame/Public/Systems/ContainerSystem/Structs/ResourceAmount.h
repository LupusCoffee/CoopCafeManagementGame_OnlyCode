#pragma once
#include "CoreMinimal.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "ResourceAmount.generated.h"

USTRUCT(BlueprintType)
struct FResourceAmount
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType ResourceType = EResourceType::CoffeeBeans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount = 0.0f;

	
	FResourceAmount() = default;
	FResourceAmount(EResourceType InResourceType, float InAmount)
	{
		ResourceType = InResourceType;
		Amount = InAmount;
	}
};