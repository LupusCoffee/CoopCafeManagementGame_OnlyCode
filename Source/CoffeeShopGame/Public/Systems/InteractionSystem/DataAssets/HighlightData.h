#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Enums/ActorType.h"
#include "Engine/DataAsset.h"
#include "HighlightData.generated.h"

UCLASS(Blueprintable)
class COFFEESHOPGAME_API UHighlightData : public UDataAsset
{
	GENERATED_BODY()

public:
	//Methods
	UFUNCTION(Category = "Highlight")
	UMaterialInterface* GetHighlightMaterial();

	UFUNCTION(Category = "Highlight")
	FName GetHighlightMatColorVariableName();

	UFUNCTION(Category = "Highlight")
	FLinearColor GetColor(EActorType ActorType);

protected:
	//Variables --> Edit
	UPROPERTY(EditAnywhere, Category = "Highlight")
	UMaterialInterface* HighlightMaterial;

	UPROPERTY(EditAnywhere, Category = "Highlight")
	FName HighlightMatColorVariableName = "Color";

	UPROPERTY(EditAnywhere, Category = "Highlight")
	FLinearColor DefaultColor;
	
	UPROPERTY(EditAnywhere, Category = "Highlight")
	TMap<EActorType, FLinearColor> HighlightColors;
};
