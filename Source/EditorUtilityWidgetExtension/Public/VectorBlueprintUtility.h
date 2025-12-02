// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VectorBlueprintUtility.generated.h"

/**
 * 
 */
UCLASS()
class EDITORUTILITYWIDGETEXTENSION_API UVectorBlueprintUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FIntVector Add(FIntVector First, FIntVector Second);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FIntVector Subtract(FIntVector First, FIntVector Second);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool Equals(FIntVector First, FIntVector Second);
};
