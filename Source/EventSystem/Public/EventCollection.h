// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EventSystemStructs.h"
#include "UObject/ObjectSaveContext.h"
#include "EventCollection.generated.h"

/**
 * 
 */
UCLASS()
class EVENTSYSTEM_API UEventCollection : public UDataAsset
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	DECLARE_DELEGATE(OnPostUndoSignature)
#endif
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEventData> Events;

#if WITH_EDITORONLY_DATA

	virtual void PostEditUndo() override
	{
		OnPostUndoDelegate.ExecuteIfBound();
	}

	OnPostUndoSignature OnPostUndoDelegate;

	UPROPERTY(EditAnywhere)
	TMap<FName, int> EventDelegateReferenceCounts;
#endif
};
