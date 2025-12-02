#include "Core/Framework/GameModesStatesInstances/CoffeeShopGameInstance.h"

void UCoffeeShopGameInstance::Init()
{
	Super::Init();

	RecipeSubsystem = GetSubsystem<URecipeSubsystem>();
	RecipeSubsystem->InitRecipeSubsystem(AvailableRecipes, PreDiscoveredRecipes);
}
