// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Enums/DayEnum.h"
#include "TimeStructs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTime
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite)
	int Hour;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite)
	int Minute;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite)
	int Second;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite)
	int Millisecond;
};

USTRUCT(BlueprintType)
struct FDate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Year;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Month;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Week;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDayEnum WeekDay;
};

USTRUCT(BlueprintType)
struct FTimeDate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDate Date;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTime Time;
};
