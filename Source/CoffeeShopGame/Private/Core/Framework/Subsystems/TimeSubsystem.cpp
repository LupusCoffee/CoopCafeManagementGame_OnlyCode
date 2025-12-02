// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Framework/Subsystems/TimeSubsystem.h"

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
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULoadingScreenManager, STATGROUP_Tickables);
}

void UTimeSubsystem::Tick(float DeltaTime)
{
	Milliseconds += DeltaTime * 1000 * TimeMult;

	Seconds += Milliseconds / 1000;
	Milliseconds = Milliseconds % 1000;

	Minutes += Seconds / 60;
	Seconds = Seconds % 60;

	Hours += Minutes / 60;
	Minutes = Minutes % 60;

	Days += Hours / 24;
	DaysInWeek += Hours / 24;
	Hours = Hours % 24;

	if (DaysInWeek >= 8)
		DaysInWeek = 1;

	int PreviousMonth = Months;
	Months += Days / DaysInMonth;
	Days = Days % DaysInMonth == 0 ? 1 : Days;

	if (PreviousMonth != Months)
		EvaluateDaysInMonth();

	int PreviousYear = Years;
	Years += Months / 13;
	Months = Months % 13 == 0 ? 1 : Months;

	if (PreviousYear != Years)
	{
		EvaluateLeapYear();
	}
}

void UTimeSubsystem::SetDate(int inYears, int inMonths, int inDays)
{
	Years = FMath::Max(inYears, 0);
	EvaluateLeapYear();

	Months = FMath::Clamp(inMonths, 1, 12);
	EvaluateDaysInMonth();

	Days = FMath::Clamp(inDays, 1, DaysInMonth);
	EvaluateDayInWeek();
}

void UTimeSubsystem::SetTime(int inHours, int inMinutes, int inSeconds, int inMilliseconds)
{
	Hours = FMath::Clamp(inHours, 0, 23);
	Minutes = FMath::Clamp(inMinutes, 0, 59);
	Seconds = FMath::Clamp(inSeconds, 0, 59);
	Milliseconds = FMath::Clamp(inMilliseconds, 0, 999);
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

	CurrentTime.Hour = Hours;
	CurrentTime.Minute = Minutes;
	CurrentTime.Second = Seconds;
	CurrentTime.Millisecond = Milliseconds;

	return CurrentTime;
}

FDate UTimeSubsystem::GetDate()
{
	FDate CurrentDate;

	CurrentDate.Year = Years;
	CurrentDate.Month = Months;
	CurrentDate.Day = Days;
	CurrentDate.WeekDay = (EDayEnum)(DaysInWeek - 1);

	return CurrentDate;
}

bool UTimeSubsystem::EvaluateLeapYear()
{
	bIsLeapYear = (Years % 4 == 0) && (!(Years % 100 == 0) || (Years % 400 == 0));

	return bIsLeapYear;
}

int UTimeSubsystem::EvaluateDaysInMonth()
{
	if (Months == 2)
	{
		DaysInMonth = 1 + (bIsLeapYear ? 29 : 28);
		return DaysInMonth;
	}

	DaysInMonth = 1 + (((Months > 7) ^ (Months % 2 == 0)) ? 30 : 31);

	return DaysInMonth;
}

int UTimeSubsystem::EvaluateDayInWeek()
{
	int YY = (Years % 100);
	int YC = ((YY / 4) + YY) % 7;
	int MC = MonthCodes[Months - 1];
	int CC = 6 - (2 * (((Years - 1) / 100) % 4));
	int WeekDay = (YC + MC + CC + Days - 1 - (bIsLeapYear ? 1 : 0)) % 7;
	DaysInWeek = WeekDay + 1;

	return DaysInWeek;
}
