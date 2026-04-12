// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EventSystemSetting.h"

#include "EventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEventTriggeredSignature, const FEventTag&, EventName, const FDataContainer&, DataContainer);

class UTimeSubsystem;
class AEventAnnouncer;

/**
 * 
 */
UCLASS()
class EVENTSYSTEM_API UEventSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	static AEventAnnouncer* GetEventAnnouncer();

private:
	UPROPERTY(BlueprintAssignable)
	FOnEventTriggeredSignature OnEventTriggeredDelegate;

	UPROPERTY()
	UTimeSubsystem* TimeSubsystem;

	TMap<int, TArray<FEventData>> OneTimeEvents;

	UPROPERTY()
	TArray<FEventData> RepeatingEvents;

	TArray<FEventData*> TriggeredEvents;
	int Day;

	static TObjectPtr<AEventAnnouncer> Announcer;

private:
	int EvaluateTotalDays(FDate& Date);
};
