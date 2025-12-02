// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "Core/Data/Structs/TimeStructs.h"

#include "TimeSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UTimeSubsystem : public UTickableWorldSubsystem
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

private:
	bool EvaluateLeapYear();
	int EvaluateDaysInMonth();
	int EvaluateDayInWeek();

private:
	int DurationOfInGameDay;
	int Milliseconds, Seconds, Minutes, Hours;
	int Days, Months, Years;
	int DaysInWeek;
	float TimeMult;

	bool bIsLeapYear = false;
	int DaysInMonth = 30;

	int MonthCodes[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };
};
