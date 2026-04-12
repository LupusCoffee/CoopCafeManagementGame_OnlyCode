#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "BaseRecipeData.generated.h"

USTRUCT(BlueprintType)
struct FResourcePercentageAsData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType ResourceType = EResourceType::CoffeeBeans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float Percentage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MarginOfError = 0.0f;

	FResourcePercentageAsData() = default;
	FResourcePercentageAsData(EResourceType InResourceType, float InPercentage, float InMarginOfError)
	{
		ResourceType = InResourceType;
		Percentage = InPercentage;
		MarginOfError = InMarginOfError;
	}

	bool operator==(const FResourcePercentageAsData& Other) const
	{
		bool ReturnBool = ResourceType == Other.ResourceType
						  && FMath::IsNearlyEqual(Percentage, Other.Percentage)
						  && FMath::IsNearlyEqual(MarginOfError, Other.MarginOfError);
		return ReturnBool;
	}
	
	bool operator!=(const FResourcePercentageAsData& Other) const
	{
		bool ReturnBool = !(*this == Other);
		return ReturnBool;
	}
};

UCLASS()
class COFFEESHOPGAME_API UBaseRecipeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourcePercentageAsData> RecipeResourceData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EResourceType ResultingResource = EResourceType::CoffeeBeans;
};
