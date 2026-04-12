// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeSubsystem.h"
#include "TimeDateUtility.h"

void UTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTimeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TStatId UTimeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeSubsystem, STATGROUP_Tickables);
}

void UTimeSubsystem::Tick(float DeltaTime)
{
	Millisecond += DeltaTime * 1000 * TimeMult;

	Second += Millisecond / 1000;
	Millisecond = Millisecond % 1000;

	Minute += Second / 60;
	Second = Second % 60;

	Hour += Minute / 60;
	Minute = Minute % 60;

	Day += Hour / 24;
	DayInWeek += Hour / 24;
	Hour = Hour % 24;

	if (DayInWeek >= 8)
		DayInWeek = 1;

	int PreviousMonth = Month;
	Month += Day / DaysInMonth;
	Day = (Day % (DaysInMonth + 1) == 0) ? 1 : Day;

	if (PreviousMonth != Month)
		DaysInMonth = UTimeDateUtility::EvaluateDaysInMonth(Year, Month);

	int PreviousYear = Year;
	Year += Month / 13;
	Month = Month % 13 == 0 ? 1 : Month;

	if (PreviousYear != Year)
	{
		bIsLeapYear = UTimeDateUtility::EvaluateLeapYear(Year);
	}
}

void UTimeSubsystem::SetDate(int inYear, int inMonth, int inDay)
{
	IsInitialized = true;
	Year = FMath::Max(inYear, 0);
	bIsLeapYear = UTimeDateUtility::EvaluateLeapYear(Year);

	Month = FMath::Clamp(inMonth, 1, 12);
	DaysInMonth = UTimeDateUtility::EvaluateDaysInMonth(Year, Month);

	Day = FMath::Clamp(inDay, 1, DaysInMonth);
	DayInWeek = UTimeDateUtility::EvaluateWeekDay(Year, Month, Day);
}

void UTimeSubsystem::SetTime(int inHours, int inMinutes, int inSeconds, int inMilliseconds)
{
	Hour = FMath::Clamp(inHours, 0, 23);
	Minute = FMath::Clamp(inMinutes, 0, 59);
	Second = FMath::Clamp(inSeconds, 0, 59);
	Millisecond = FMath::Clamp(inMilliseconds, 0, 999);
}

void UTimeSubsystem::SetDurationOfInGameDay(FTime Duration)
{
	DurationOfInGameDay = Duration.Hour * 3600 + Duration.Minute * 60 + Duration.Second;

			// Duration of real-time day in seconds
	TimeMult = 86400 / DurationOfInGameDay;
}

void UTimeSubsystem::SetDurationOfInGameDay(int inHours, int inMinutes, int inSeconds)
{
	DurationOfInGameDay = inHours * 3600 + inMinutes * 60 + inSeconds;

			// Duration of real-time day in seconds
	TimeMult = 86400 / DurationOfInGameDay;
}

FTime UTimeSubsystem::GetTime()
{
	FTime CurrentTime;

	CurrentTime.Hour = Hour;
	CurrentTime.Minute = Minute;
	CurrentTime.Second = Second;

	return CurrentTime;
}

FDate UTimeSubsystem::GetDate()
{
	FDate CurrentDate;

	CurrentDate.Year = Year;
	CurrentDate.Month = Month;
	CurrentDate.Day = Day;
	CurrentDate.WeekDay = (EDayEnum)(DayInWeek);

	return CurrentDate;
}

FTimeDate UTimeSubsystem::GetTimeDate()
{
	FTimeDate CurrentTimeDate;

	CurrentTimeDate.Date.Year = Year;
	CurrentTimeDate.Date.Month = Month;
	CurrentTimeDate.Date.Day = Day;
	CurrentTimeDate.Date.WeekDay = (EDayEnum)(DayInWeek);

	CurrentTimeDate.Time.Hour = Hour;
	CurrentTimeDate.Time.Minute = Minute;
	CurrentTimeDate.Time.Second = Second;

	return CurrentTimeDate;
}
