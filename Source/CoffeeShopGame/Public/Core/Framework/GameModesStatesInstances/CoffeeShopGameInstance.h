#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Systems/Items/Subsystems/RecipeSubsystem.h"
#include "CoffeeShopGameInstance.generated.h"

UCLASS()
class COFFEESHOPGAME_API UCoffeeShopGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

protected:
	//Variables --> Editable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RecipeSubsystem")
	URecipeArrayDataAsset* AvailableRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RecipeSubsystem")
	URecipeArrayDataAsset* PreDiscoveredRecipes;
	
	//Variables --> Hidden, Subsystems
	UPROPERTY()
	URecipeSubsystem* RecipeSubsystem;
};
