#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/DataAssets/ConversionData.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/DataAssets/RecipeArrayDataAsset.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Structs/ResourceConversionEntry.h"
#include "ResourceConverterSubsystem.generated.h"
class UContainerObject;
struct FResourceAmount;

USTRUCT(BlueprintType)
struct FRecipe
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourcePercentageAsData> RecipeResourceData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EResourceType ResultingResource = EResourceType::None;

	FRecipe() = default;
		
	FRecipe(UBaseRecipeData* RecipeData)
	{
		RecipeResourceData = RecipeData->RecipeResourceData;
		ResultingResource = RecipeData->ResultingResource;
	}
	
	FRecipe(TArray<FResourcePercentageAsData> InRecipeResourceData, EResourceType InResultingResource)
	{
		RecipeResourceData = InRecipeResourceData;
		ResultingResource = InResultingResource;
	}

	FORCEINLINE bool operator==(const FRecipe& Other) const
	{
		return this->ResultingResource == Other.ResultingResource &&
			   this->RecipeResourceData == Other.RecipeResourceData;
	}

	FORCEINLINE bool operator!=(const FRecipe& Other) const
	{
		bool ReturnBool = !(*this == Other);
		return ReturnBool;
	}
};

USTRUCT(BlueprintType)
struct FResourcePercentageAsContent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType ResourceType = EResourceType::CoffeeBeans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentPercentage = 0.0f;

	FResourcePercentageAsContent() = default;
	FResourcePercentageAsContent(EResourceType InResourceType, float CurrentVolume, float MaxVolume)
	{
		ResourceType = InResourceType;
		CurrentPercentage = CurrentVolume/MaxVolume;
	}
};

UCLASS()
class COFFEESHOPGAME_API UResourceConverterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//Methods --> Setup
	// core
	UFUNCTION(BlueprintCallable)
	void InitSubsystem(URecipeArrayDataAsset* InAvailableRecipes, URecipeArrayDataAsset* PreDiscoveredRecipes, UConversionData* InGrindConversionData);
	
	// recipe
	UFUNCTION()
	void InitRecipe(URecipeArrayDataAsset* InAvailableRecipes, URecipeArrayDataAsset* PreDiscoveredRecipes);
	
	UFUNCTION(BlueprintCallable)
	bool TryAddDiscoveredRecipe(FRecipe Recipe);

	// grind
	UFUNCTION()
	void InitGrind(UConversionData* GrindConversionData);


	//Methods --> Try Convert
	UFUNCTION(BlueprintCallable)
	bool TryConvertFromRecipe(TArray<FResourceAmount>& ContentAsAmountsToConvert);

	UFUNCTION(BlueprintCallable)
	bool TryMoveContentAndGrind(float AmountToMove, UContainerObject* ContainerA, UContainerObject* ContainerB);

	UFUNCTION(BlueprintCallable)
	void TryGrindContent(UContainerObject* Container);


	//Methods --> Public Helpers, Grind
	UFUNCTION(BlueprintCallable)
	EResourceType TryGetGrindConversionOutputResource(EResourceType InputResourceType);
	

	//Methods --> Miscellanious
	UFUNCTION(BlueprintCallable)
	bool TryMoveContent(float AmountToMove, UContainerObject* ContainerA, UContainerObject* ContainerB);

	

protected:
	//Variables --> Recipe
	UPROPERTY(BlueprintReadOnly)
	URecipeArrayDataAsset* AvailableRecipesData;

	UPROPERTY(BlueprintReadOnly)
	TArray<FRecipe> DiscoveredRecipes;

	UPROPERTY(BlueprintReadOnly) //this is what we use to compare content
	TArray<FRecipe> BrokenDownRecipes;

	UPROPERTY(BlueprintReadOnly)
	TArray<EResourceType> BaseIngredients;

	
	//Variables --> Grind
	UPROPERTY()
	TArray<FResourceConversionEntry> AvailableGrindConversions;


	
	//Methods --> Recipe Helpers
	UFUNCTION(BlueprintCallable)
	bool FindEquivalentRecipeResource(TArray<FResourcePercentageAsContent> ContentToCompare, EResourceType& ResultResource);
	
	UFUNCTION()
	bool Coincide(TArray<FResourcePercentageAsContent> ContentToCompare, FRecipe RecipeToCompare);
	
	UFUNCTION()
	FRecipe BreakDownRecipe(FRecipe InRecipe);

	UFUNCTION()
	TArray<FResourcePercentageAsContent> BreakDownContent(TArray<FResourcePercentageAsContent> Content);
};
