#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "ResourceConversionEntry.generated.h"
enum class EResourceType : uint8;

USTRUCT(BlueprintType)
struct FResourceConversionEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType InputResourceType = EResourceType::CoffeeBeans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType OutputResourceType = EResourceType::CoffeeBeans;

	FResourceConversionEntry() = default;
};
