// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorUtility.generated.h"

class UBlueprint;

/**
 *
 */
UCLASS()
class EDITORUTILITYWIDGETEXTENSION_API UEditorUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SetObjectPropertyByName(UBlueprint* BlueprintAsset, FString PropertyName, UObject* Object);
	UFUNCTION(BlueprintCallable)
	static TArray<UObject*> GetAllAssetOfClass(UClass* AssetClass, bool SearchSubclass = false);
};
