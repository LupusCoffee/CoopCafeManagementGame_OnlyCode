#include "Systems/Items/Subsystems/RecipeSubsystem.h"

void URecipeSubsystem::InitRecipeSubsystem(URecipeArrayDataAsset* InAvailableRecipes, URecipeArrayDataAsset* PreDiscoveredRecipes)
{
	if (InAvailableRecipes) AvailableRecipesData = InAvailableRecipes;
	if (PreDiscoveredRecipes) TryAddDiscoveredRecipes(PreDiscoveredRecipes);
}

void URecipeSubsystem::TryAddDiscoveredRecipes(URecipeArrayDataAsset* RecipeArrayData)
{
	if (!RecipeArrayData) return;
	
	for (auto RecipeData : RecipeArrayData->Recipes)
	{
		if (DiscoveredRecipes.Contains(RecipeData))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "WARNING: Tried to Discover Already Discovered Recipe");
			continue;
		}
		
		DiscoveredRecipes.Add(RecipeData);
	}
}

bool URecipeSubsystem::TryAddDiscoveredRecipe(UBaseRecipeData* RecipeData)
{
	if (!RecipeData) return false;
	
	if (!AvailableRecipesData->Recipes.Contains(RecipeData))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "WARNING: Tried to Discover Non-Available Recipe");
		return false;
	}

	if (DiscoveredRecipes.Contains(RecipeData))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "WARNING: Tried to Discover Already Discovered Recipe");
		return false;
	}

	DiscoveredRecipes.Add(RecipeData);
	return true;
}

TArray<UBaseRecipeData*> URecipeSubsystem::GetAvailableRecipes()
{
	return AvailableRecipesData->Recipes;
}