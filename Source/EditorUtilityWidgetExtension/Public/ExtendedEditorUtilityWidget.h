// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ExtendedEditorUtilityWidget.generated.h"

/**
 *
 */
UCLASS()
class EDITORUTILITYWIDGETEXTENSION_API UExtendedEditorUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionChangedDelegate, UObject*, NewSelection);

	UFUNCTION(BlueprintCallable)
	void Init();

	UPROPERTY(BlueprintAssignable)
	FOnSelectionChangedDelegate OnSelectionChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", AdvancedDisplay, meta = (AllowPrivateAccess = true, EditCondition = "bIsRecipe"))
	UTexture2D* CraftedItemIcon;

private:
	void BroadcastSelectionChanged(UObject* NewSelection);
};
