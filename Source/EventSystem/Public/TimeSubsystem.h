// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TimeStructs.h"
#include "TimeSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class EVENTSYSTEM_API UTimeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override;

	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetDate(int inYears, int inMonths, int inDays);
	UFUNCTION(BlueprintCallable)
	void SetTime(int inHours, int inMinutes, int inSeconds, int inMilliseconds);
	UFUNCTION(BlueprintCallable)
	void SetDurationOfInGameDay(int inHours, int inMinutes, int inSeconds);
	void SetDurationOfInGameDay(FTime Duration);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTime GetTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FDate GetDate();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTimeDate GetTimeDate();
	
	bool IsInitialized = false;

private:

private:
	int DurationOfInGameDay;
	int Millisecond, Second, Minute, Hour;
	int Day, Month, Year;
	int DayInWeek;
	float TimeMult;

	bool bIsLeapYear = false;
	int DaysInMonth = 30;
	
};
