// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TimeStructs.h"
#include "Containers/StaticArray.h"
#include "EventSystemStructs.generated.h"


struct FTimeDate;

/**
 *
 */

USTRUCT(BlueprintType)
struct EVENTSYSTEM_API FEventTag
{
	GENERATED_USTRUCT_BODY()

public:
	bool operator==(FEventTag const& Other) const
	{
		return TagName == Other.TagName;
	}

	bool operator!=(FEventTag const& Other) const
	{
		return TagName != Other.TagName;
	}

	FORCEINLINE friend uint32 GetTypeHash(const FEventTag& Tag)
	{
		return GetTypeHash(Tag.TagName);
	}

	FName GetTagName() { return TagName; }
	void SetTagName(FName InTagName) { TagName = InTagName; }

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	FName TagName;
};

USTRUCT(BlueprintType)
struct EVENTSYSTEM_API FDataTag
{
	GENERATED_USTRUCT_BODY()

public:
	FDataTag() = default;
	FDataTag(FName InTagName)
	{
		TagName = InTagName;
	}

	bool operator==(FDataTag const& Other) const
	{
		return TagName == Other.TagName;
	}

	bool operator!=(FDataTag const& Other) const
	{
		return TagName != Other.TagName;
	}

	FORCEINLINE friend uint32 GetTypeHash(const FDataTag& Tag)
	{
		return GetTypeHash(Tag.TagName);
	}

	FName GetTagName() const { return TagName; }

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	FName TagName;
};

USTRUCT(BlueprintType)
struct EVENTSYSTEM_API FDataContainer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<FDataTag, double> Data;
};

USTRUCT(BlueprintType)
struct FRepeatList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, EditFixedSize)
	TArray<bool> RepeatingDays = { false, false, false, false, false, false, false };
};

USTRUCT(BlueprintType)
struct EVENTSYSTEM_API FEventData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FEventTag Event;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTimeDate TriggerTime;

	UPROPERTY(EditAnywhere, EditFixedSize)
	TArray<bool> RepeatingDays = {false, false, false, false, false, false, false};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FDataContainer DataContainer;

	bool bTriggered = false;
};
