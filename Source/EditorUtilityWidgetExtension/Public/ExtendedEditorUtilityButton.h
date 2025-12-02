// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidgetComponents.h"
#include "ExtendedEditorUtilityButton.generated.h"

/**
 * 
 */
UCLASS()
class EDITORUTILITYWIDGETEXTENSION_API UExtendedEditorUtilityButton : public UEditorUtilityButton
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelfClicked, UExtendedEditorUtilityButton*, Caller);

	UFUNCTION(BlueprintCallable)
	void Init();

	UPROPERTY(BlueprintAssignable)
	FOnSelfClicked OnSelfClicked;

private:
	UFUNCTION()
	void BroadcastOnSelfClicked();
};
