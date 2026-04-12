// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DayEnum.h"
#include "TimeDateUtility.h"
#include "TimeStructs.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FTime
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Hour = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Minute = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Second = 0;

	FTime() = default;

	FTime& operator =(const FTime& b)
	{
		Hour = b.Hour;
		Minute = b.Minute;
		Second = b.Second;

		return *this;
	}

	bool operator ==(const FTime& Other) const
	{
		int CurrentInSeconds = (((((this->Hour * 60) + this->Minute) * 60) + this->Second));
		int OtherInSeconds = (((((Other.Hour * 60) + Other.Minute) * 60) + Other.Second));

		if (CurrentInSeconds == OtherInSeconds)
			return true;

		return false;
	}

	bool operator >=(const FTime& Other) const
	{
		int CurrentInSeconds = (((((this->Hour * 60) + this->Minute) * 60) + this->Second));
		int OtherInSeconds = (((((Other.Hour * 60) + Other.Minute) * 60) + Other.Second));
		
		if (CurrentInSeconds >= OtherInSeconds)
			return true;

		return false;
	}

	bool operator >(const FTime& Other) const
	{
		int CurrentInSeconds = (((((this->Hour * 60) + this->Minute) * 60) + this->Second));
		int OtherInSeconds = (((((Other.Hour * 60) + Other.Minute) * 60) + Other.Second));

		if (CurrentInSeconds > OtherInSeconds)
			return true;

		return false;
	}

	bool operator <=(const FTime& Other) const
	{
		int CurrentInSeconds = (((((this->Hour * 60) + this->Minute) * 60) + this->Second));
		int OtherInSeconds = (((((Other.Hour * 60) + Other.Minute) * 60) + Other.Second));

		if (CurrentInSeconds <= OtherInSeconds)
			return true;

		return false;
	}

	bool operator <(const FTime& Other) const
	{
		int CurrentInSeconds = (((((this->Hour * 60) + this->Minute) * 60) + this->Second));
		int OtherInSeconds = (((((Other.Hour * 60) + Other.Minute) * 60) + Other.Second));

		if (CurrentInSeconds < OtherInSeconds)
			return true;

		return false;
	}
};

USTRUCT(BlueprintType)
struct FDate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Year = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Month = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Day = 1;

	UPROPERTY(BlueprintReadWrite, Transient)
	int Week;

	UPROPERTY(BlueprintReadWrite, Transient)
	EDayEnum WeekDay;

	FDate() = default;

	FDate& operator ++(int)
	{
		int DaysInMonth = UTimeDateUtility::EvaluateDaysInMonth(Year, Month);

		Day++;
		 
		if (Day > DaysInMonth)
		{
			Month++;
			Day = 1;

			if (Month > 12)
			{
				Month = 1;
				Year++;
			}
		}

		return *this;
	}

	FDate& operator =(const FDate& b)
	{
		Day = b.Day;
		Month = b.Month;
		Year = b.Year;
		WeekDay = b.WeekDay;
		Week = b.Week;

		return *this;
	}

	bool operator ==(const FDate& b) const
	{
		return (this->Day == b.Day && this->Month == b.Month && this->Year == b.Year);
	}

	bool operator <=(const FDate& b) const
	{
		if (Year < b.Year)
			return true;

		if (Year == b.Year && Month < b.Month)
			return true;

		if (Month == b.Month && Day < b.Day)
			return true;

		if (Day == b.Day)
			return true;

		return false;
	}

	bool operator <(const FDate& b) const
	{
		if (Year < b.Year)
			return true;

		if (Year == b.Year && Month < b.Month)
			return true;

		if (Month == b.Month && Day < b.Day)
			return true;

		return false;
	}

	bool operator >=(const FDate& b) const
	{
		if (Year > b.Year)
			return true;

		if (Year == b.Year && Month > b.Month)
			return true;

		if (Month == b.Month && Day > b.Day)
			return true;

		if (Day == b.Day)
			return true;

		return false;
	}

	bool operator >(const FDate& b) const
	{
		if (Year > b.Year)
			return true;

		if (Year == b.Year && Month > b.Month)
			return true;

		if (Month == b.Month && Day > b.Day)
			return true;

		return false;
	}
};

USTRUCT(BlueprintType)
struct EVENTSYSTEM_API FTimeDate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDate Date;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTime Time;

	FTimeDate& operator =(const FTimeDate& b)
	{
		Time = b.Time;
		Date = b.Date;

		return *this;
	}

	bool operator ==(const FTimeDate& b) const
	{
		return (this->Date == b.Date && this->Time == b.Time);
	}

	bool operator <(const FTimeDate& b) const
	{
		if (Date < b.Date)
			return true;
		else if (Date == b.Date)
			if (Time < b.Time)
				return true;

		return false;
	}

	bool operator <=(const FTimeDate& b) const
	{
		if (Date < b.Date)
			return true;
		else if (Date == b.Date)
			if (Time <= b.Time)
				return true;

		return false;
	}

	bool operator >(const FTimeDate& b) const
	{
		if (Date > b.Date)
			return true;
		else if (Date == b.Date)
			if (Time > b.Time)
				return true;

		return false;
	}

	bool operator >=(const FTimeDate& b) const
	{
		if (Date > b.Date)
			return true;
		else if (Date == b.Date)
			if (Time >= b.Time)
				return true;

		return false;
	}

	const static FTimeDate ZeroTimeDate;
};

USTRUCT(BlueprintType)
struct EVENTSYSTEM_API FTimeFrame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimeDate Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimeDate End;

	bool DateOnly;

	FTimeFrame() = default;

	FTimeFrame(const FTimeDate& InStart, const FTimeDate& InEnd, bool InDateOnly = true)
	{
		Start = InStart;
		End = InEnd;
		DateOnly = InDateOnly;
	}

	bool IsInTimeFrame(const FTimeDate& TimeDate)
	{
		if (DateOnly)
			return (TimeDate.Date >= Start.Date && TimeDate.Date <= End.Date);
		else
			return (TimeDate >= Start && TimeDate <= End);
	}
};