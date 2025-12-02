#pragma once
#include "CoreMinimal.h"
#include "Systems/Items/Enums/ResourceType.h"
#include "ResourceStack.generated.h"

USTRUCT(BlueprintType)
struct FResourceStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType ResourceType = EResourceType::CoffeeBeans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount = 0.0f;

	FResourceStack() = default;
	FResourceStack(EResourceType InResourceType, float InAmount)
	{
		ResourceType = InResourceType;
		Amount = InAmount;
	}
};