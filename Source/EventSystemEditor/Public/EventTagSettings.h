#pragma once

#include "CoreMinimal.h"
#include "EventTagSettings.generated.h"

USTRUCT()
struct FDefinedEvent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString Event;

	UPROPERTY(EditAnywhere)
	TArray<FString> Data;
};

UCLASS(config = EventTagList, MinimalAPI)
class UTagList : public UObject
{
	GENERATED_UCLASS_BODY()

	/** Relative path to the ini file that is backing this list */
	UPROPERTY()
	FString ConfigFileName;

	/** List of tags saved to this file */
	UPROPERTY(config, EditAnywhere, Category = EventTag)
	TArray<FDefinedEvent> EventList;
};

UCLASS(config = EventTags, defaultconfig, MinimalAPI)
class UEventTagSettings : public UTagList
{
	GENERATED_UCLASS_BODY()

	TArray<FDefinedEvent> GetEventList() const { return EventList; }
	TArray<FString> GetEventTagList() const
	{
		TArray<FString> EventTagList;
		for (const FDefinedEvent& DefinedEvent : EventList)
		{
			EventTagList.Add(DefinedEvent.Event);
		}
		return EventTagList;
	}
	TArray<FString> GetDataTagList() const
	{
		TArray<FString> DataTagList;
		for (const FDefinedEvent& DefinedEvent : EventList)
		{
			DataTagList.Append(DefinedEvent.Data);
		}
		return DataTagList;
	}
};