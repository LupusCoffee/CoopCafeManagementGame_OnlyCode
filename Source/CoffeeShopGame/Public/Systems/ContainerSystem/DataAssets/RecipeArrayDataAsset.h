#pragma once

#include "CoreMinimal.h"
#include "BaseRecipeData.h"
#include "Engine/DataAsset.h"
#include "RecipeArrayDataAsset.generated.h"

UCLASS()
class COFFEESHOPGAME_API URecipeArrayDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UBaseRecipeData*> Recipes;
};
