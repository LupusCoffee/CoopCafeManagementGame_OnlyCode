// Fill out your copyright notice in the Description page of Project Settings.

#include "AILifecycleEvents.h"
#include "Engine/World.h"

// Initialize static map
TMap<TWeakObjectPtr<UWorld>, TObjectPtr<UAILifecycleEvents>> UAILifecycleEvents::WorldInstances;

void UAILifecycleEvents::CleanupInvalidWorlds()
{
	TArray<TWeakObjectPtr<UWorld>> StaleKeys;
	for (const auto& Pair : WorldInstances)
	{
		if (!Pair.Key.IsValid() || !IsValid(Pair.Value))
		{
			StaleKeys.Add(Pair.Key);
		}
	}

	for (const TWeakObjectPtr<UWorld>& Key : StaleKeys)
	{
		WorldInstances.Remove(Key);
	}
}

UAILifecycleEvents* UAILifecycleEvents::Get(UWorld* World)
{
	if (!World || World->bIsTearingDown || !World->IsGameWorld())
	{
		return nullptr;
	}

	CleanupInvalidWorlds();

	TWeakObjectPtr<UWorld> WeakWorld(World);
	
	// Check if instance exists
	if (TObjectPtr<UAILifecycleEvents>* ExistingInstance = WorldInstances.Find(WeakWorld))
	{
		if (IsValid(*ExistingInstance))
		{
			return ExistingInstance->Get();
		}

		WorldInstances.Remove(WeakWorld);
	}

	// Create new world-owned instance so it survives GC for the world lifetime.
	UAILifecycleEvents* NewInstance = NewObject<UAILifecycleEvents>(World);
	WorldInstances.Add(WeakWorld, NewInstance);
	
	return NewInstance;
}

void UAILifecycleEvents::BroadcastAISpawned(UWorld* World, APawn* Pawn, const FString& DebugName)
{
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	if (UAILifecycleEvents* Events = Get(World))
	{
		Events->OnAISpawned.Broadcast(Pawn, DebugName);
	}
}

void UAILifecycleEvents::BroadcastAIDestroyed(UWorld* World, APawn* Pawn)
{
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	if (UAILifecycleEvents* Events = Get(World))
	{
		Events->OnAIDestroyed.Broadcast(Pawn);
	}
}

void UAILifecycleEvents::BroadcastAIActivated(UWorld* World, APawn* Pawn)
{
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	if (UAILifecycleEvents* Events = Get(World))
	{
		Events->OnAIActivated.Broadcast(Pawn);
	}
}

void UAILifecycleEvents::BroadcastAIDeactivated(UWorld* World, APawn* Pawn)
{
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	if (UAILifecycleEvents* Events = Get(World))
	{
		Events->OnAIDeactivated.Broadcast(Pawn);
	}
}
