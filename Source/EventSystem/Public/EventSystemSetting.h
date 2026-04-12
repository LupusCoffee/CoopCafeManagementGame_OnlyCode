// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EventCollection.h"
#include "EventSystemSetting.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Event Setting"))
class EVENTSYSTEM_API UEventSystemSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UEventCollection> EventCollection;

	UEventSystemSetting() = default;

	UFUNCTION(BlueprintCallable)
	static const UEventSystemSetting* GetEventSettings();
};
