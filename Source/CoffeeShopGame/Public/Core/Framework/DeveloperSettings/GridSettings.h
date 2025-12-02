// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GridSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Grid Settings"))
class COFFEESHOPGAME_API UGridSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FVector2D CellSize;

	UGridSettings() = default;

	UFUNCTION(BlueprintCallable)
	static const UGridSettings* GetGridSettings();
};
