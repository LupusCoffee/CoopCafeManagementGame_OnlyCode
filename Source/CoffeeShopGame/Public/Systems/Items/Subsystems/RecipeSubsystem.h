#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Systems/Items/DataAssets/RecipeArrayDataAsset.h"
#include "RecipeSubsystem.generated.h"

UCLASS()
class COFFEESHOPGAME_API URecipeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitRecipeSubsystem(URecipeArrayDataAsset* InAvailableRecipes, URecipeArrayDataAsset* PreDiscoveredRecipes);
	
	UFUNCTION(BlueprintCallable)
	void TryAddDiscoveredRecipes(URecipeArrayDataAsset* RecipeArrayData);
	
	UFUNCTION(BlueprintCallable)
	bool TryAddDiscoveredRecipe(UBaseRecipeData* RecipeData);
	
	UFUNCTION(BlueprintCallable)
	TArray<UBaseRecipeData*> GetAvailableRecipes();

protected:
	UPROPERTY(BlueprintReadOnly)
	URecipeArrayDataAsset* AvailableRecipesData;

	UPROPERTY(BlueprintReadOnly)
	TArray<UBaseRecipeData*> DiscoveredRecipes;
};
