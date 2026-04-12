// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSubsystem.h"
#include "TimeSubsystem.h"
#include "EventAnnouncer.h"

TObjectPtr<AEventAnnouncer> UEventSubsystem::Announcer = nullptr;

void UEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* World = GetWorld();

#if WITH_EDITOR
	if (!World->IsGameWorld())
		return;
#endif

	FSoftClassPath AnnouncerPath(TEXT("/Game/EventSystem/BP_EventAnnouncer.BP_EventAnnouncer_C"));
	UClass* AnnoucerClass = AnnouncerPath.TryLoadClass<AActor>();
	checkf(AnnoucerClass != nullptr, TEXT("expected non-null class"));
	Announcer = World->SpawnActor<AEventAnnouncer>(AnnoucerClass);

	const UEventSystemSetting* Setting = UEventSystemSetting::GetEventSettings();

	if (Setting)
	{
		if (UEventCollection* EventCollection = Setting->EventCollection.LoadSynchronous())
		{
			for (auto& Event : EventCollection->Events)
			{
				if (Event.RepeatingDays.Contains(true))
					RepeatingEvents.Add(Event);
				else
					OneTimeEvents.FindOrAdd(EvaluateTotalDays(Event.TriggerTime.Date)).Add(Event);
			}
		}
	}
}

void UEventSubsystem::PostInitialize()
{
	if (UWorld* World = GetWorld())
		TimeSubsystem = World->GetSubsystem<UTimeSubsystem>();

	Day = TimeSubsystem->GetDate().Day;	
}

void UEventSubsystem::Tick(float DeltaTime)
{
	if (!TimeSubsystem->IsInitialized)
		return;
	
	FTimeDate CurrentTimeDate = TimeSubsystem->GetTimeDate();

	if(CurrentTimeDate.Date.Day != Day)
	{
		Day = CurrentTimeDate.Date.Day;

		for (auto& Event : TriggeredEvents)
			Event->bTriggered = false;

		TriggeredEvents.Empty();
	}

	for(auto& Event : RepeatingEvents)
	{
		if (CurrentTimeDate.Time >= Event.TriggerTime.Time && Event.RepeatingDays[(int)CurrentTimeDate.Date.WeekDay-1] && !Event.bTriggered)
		{
			OnEventTriggeredDelegate.Broadcast(Event.Event, Event.DataContainer);
			Announcer->InvokeEvent(Event.Event.GetTagName(), Event.DataContainer);
			Event.bTriggered = true;
			TriggeredEvents.Add(&Event);
		}
	}

	TArray<FEventData>* EventsOnDay = OneTimeEvents.Find(EvaluateTotalDays(CurrentTimeDate.Date));

	if (!EventsOnDay)
		return;

	for(auto& Event : *EventsOnDay)
	{
		if (CurrentTimeDate.Time >= Event.TriggerTime.Time && Event.TriggerTime.Date.Year == CurrentTimeDate.Date.Year && !Event.bTriggered)
		{
			OnEventTriggeredDelegate.Broadcast(Event.Event, Event.DataContainer);
			Announcer->InvokeEvent(Event.Event.GetTagName(), Event.DataContainer);
			Event.bTriggered = true;
			TriggeredEvents.Add(&Event);
		}
	}
}

TStatId UEventSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UEventSubsystem, STATGROUP_Tickables);
}

void UEventSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (Announcer)
	{
		Announcer->Destroy();
		Announcer = nullptr;
	}
}

AEventAnnouncer* UEventSubsystem::GetEventAnnouncer()
{
	if (Announcer)
		return Announcer;

	return nullptr;
}

int UEventSubsystem::EvaluateTotalDays(FDate& Date)
{
	bool bIsLeapYear = (Date.Year % 4 == 0) && (!(Date.Year % 100 == 0) || (Date.Year % 400 == 0));

	int TotalDays = (Date.Month - 1) * 30 + Date.Day;

	// Adjust for leap year if current month is after February
	TotalDays -= (Date.Month > 2) ? (bIsLeapYear ? 1 : 2) : 0;

	// Add days for months that have 31 days
	TotalDays += (Date.Month <= 7) ? ((Date.Month) / 2) : (Date.Month + 1 / 2);

	return TotalDays;
}
