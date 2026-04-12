// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AILifecycleEvents.generated.h"

// Forward declarations
class APawn;

/**
 * Delegate signatures for AI lifecycle events
 * These allow loose coupling between AI and the AIManager
 */

// Multicast delegate for AI spawned events
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAISpawned, APawn* /*Pawn*/, const FString& /*DebugName*/);

// Multicast delegate for AI destroyed events
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAIDestroyed, APawn* /*Pawn*/);

// Multicast delegate for AI activated events
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAIActivated, APawn* /*Pawn*/);

// Multicast delegate for AI deactivated events
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAIDeactivated, APawn* /*Pawn*/);

/**
 * Static event broadcaster for AI lifecycle events
 * This allows AI to broadcast events without knowing about the AIManager
 * The AIManager subscribes to these events to track AI automatically
 */
UCLASS()
class GOAPSYSTEM_API UAILifecycleEvents : public UObject
{
	GENERATED_BODY()

public:
	// Get the singleton instance
	static UAILifecycleEvents* Get(UWorld* World);

	// Event delegates
	FOnAISpawned OnAISpawned;
	FOnAIDestroyed OnAIDestroyed;
	FOnAIActivated OnAIActivated;
	FOnAIDeactivated OnAIDeactivated;

	// Broadcast helpers (can be called from anywhere)
	static void BroadcastAISpawned(UWorld* World, APawn* Pawn, const FString& DebugName);
	static void BroadcastAIDestroyed(UWorld* World, APawn* Pawn);
	static void BroadcastAIActivated(UWorld* World, APawn* Pawn);
	static void BroadcastAIDeactivated(UWorld* World, APawn* Pawn);

private:
	static void CleanupInvalidWorlds();

	// Per-world instances
	static TMap<TWeakObjectPtr<UWorld>, TObjectPtr<UAILifecycleEvents>> WorldInstances;
};
