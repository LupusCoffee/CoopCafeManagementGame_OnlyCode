#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Systems/Items/Enums/ResourceType.h"
#include "BaseRecipeData.generated.h"

USTRUCT(BlueprintType)
struct FResourcePercentage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType ResourceType = EResourceType::CoffeeBeans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MinPercentage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MaxPercentage = 0.0f;

	FResourcePercentage() = default;
	FResourcePercentage(EResourceType InResourceType, float InMinPercentage, float InMaxPercentage)
	{
		ResourceType = InResourceType;
		MinPercentage = InMinPercentage;
		MaxPercentage = InMaxPercentage;
	}
};

UCLASS()
class COFFEESHOPGAME_API UBaseRecipeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourcePercentage> Recipe;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EResourceType ResultingResource = EResourceType::CoffeeBeans;
};
