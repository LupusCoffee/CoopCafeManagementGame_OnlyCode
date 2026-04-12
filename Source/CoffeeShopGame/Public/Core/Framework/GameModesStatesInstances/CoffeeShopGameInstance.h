#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Systems/GamblerMacMinigame/TurnBasedManager.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PlayerPromptWidget.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Subsystems/ResourceConverterSubsystem.h"
#include "CoffeeShopGameInstance.generated.h"

class UPromptWidgetBox;
class UActionPromptSubsystem;
class UActionPromptsData;

UCLASS()
class COFFEESHOPGAME_API UCoffeeShopGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

protected:
	//Variables --> Editable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
	URecipeArrayDataAsset* AvailableRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
	URecipeArrayDataAsset* PreDiscoveredRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grind")
	UConversionData* AvailableGrindConversions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Prompt Subsystem")
	UActionPromptsData* ActionPromptsData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Prompt Subsystem")
	TSubclassOf<UPromptWidgetBox> PromptWidgetBoxClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Prompt Subsystem")
	TSubclassOf<UPlayerPromptWidget> PlayerPromptWidgetClass;
	
	
	//Variables --> Hidden, Subsystems
	UPROPERTY()
	UResourceConverterSubsystem* RecipeSubsystem;

	UPROPERTY()
	UActionPromptSubsystem* ActionPromptSubsystem = nullptr;
	
	UPROPERTY()
	UTurnBasedManager* GamblerMacMinigameManager;
};
