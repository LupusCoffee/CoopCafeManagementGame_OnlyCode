#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Systems/Interaction System/DataAssets/HighlightData.h"
#include "HighlightSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Highlighting"))
class COFFEESHOPGAME_API UHighlightSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//Methods
	UFUNCTION(BlueprintPure, Category="Highlight")
	UMaterialInterface* GetHighlightMaterial();

	UFUNCTION(BlueprintPure, Category="Highlight")
	const FName GetHighlightMatColorVariableName();

	UFUNCTION(BlueprintPure, Category="Highlight")
	const FLinearColor GetColor(EActorType ActorType);

protected:
	//Variables --> Edit
	UPROPERTY(EditAnywhere, Config, Category = "Highlight")
	const TSoftObjectPtr<UMaterialInterface> HighlightMaterial;

	UPROPERTY(EditAnywhere, Config, Category = "Highlight", meta = (ToolTip = "In the highlight material, there is a color for the highlight. The name of that color variable needs to go here."))
	FName HighlightMatColorVariableName = "Color";

	UPROPERTY(EditAnywhere, Config, Category = "Highlight")
	FLinearColor DefaultColor;
	
	UPROPERTY(EditAnywhere, Config, Category = "Highlight")
	TMap<EActorType, FLinearColor> HighlightColors;
};
