#include "Core/Framework/GameModesStatesInstances/CoffeeShopGameInstance.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionPromptSubsystem.h"

void UCoffeeShopGameInstance::Init()
{
	Super::Init();

	RecipeSubsystem = GetSubsystem<UResourceConverterSubsystem>();
	RecipeSubsystem->InitSubsystem(AvailableRecipes, PreDiscoveredRecipes, AvailableGrindConversions);
	
	ActionPromptSubsystem = GetSubsystem<UActionPromptSubsystem>();
	ActionPromptSubsystem->InitSubsystem(ActionPromptsData, PromptWidgetBoxClass, PlayerPromptWidgetClass);
}
