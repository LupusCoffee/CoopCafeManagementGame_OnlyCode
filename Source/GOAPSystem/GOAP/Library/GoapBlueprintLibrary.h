// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GoapBlueprintLibrary.generated.h"

/**
 * Blueprint function library for GOAP system utilities
 */
UCLASS()
class GOAPSYSTEM_API UGoapBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ===== Subsystem Access =====

	/** Get the GOAP World Subsystem from the world context */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Subsystem", meta = (WorldContext = "WorldContextObject"))
	static class UGoapWorldSubsystem* GetGoapWorldSubsystem(const UObject* WorldContextObject);

	// ===== Default World Knowledge =====

	/** Set a default actor that all AI should know about */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World Knowledge", meta = (WorldContext = "WorldContextObject"))
	static void SetDefaultActor(const UObject* WorldContextObject, FName Key, AActor* Actor, bool bIsGlobalKnowledge = true);

	/** Set a default location that all AI should know about */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World Knowledge", meta = (WorldContext = "WorldContextObject"))
	static void SetDefaultLocation(const UObject* WorldContextObject, FName Key, FVector Location, bool bIsGlobalKnowledge = true);

	/** Remove a default world knowledge entry */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World Knowledge", meta = (WorldContext = "WorldContextObject"))
	static void RemoveDefaultEntry(const UObject* WorldContextObject, FName Key);

	/** Get the actor associated with a key from default knowledge */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|World Knowledge", meta = (WorldContext = "WorldContextObject"))
	static AActor* GetDefaultActor(const UObject* WorldContextObject, FName Key);

	/** Get the location associated with a key from default knowledge */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World Knowledge", meta = (WorldContext = "WorldContextObject"))
	static FVector GetDefaultLocation(const UObject* WorldContextObject, FName Key, bool& bIsValid);

	// ===== Runtime Actor Assignment =====

	/** Assign a target actor to a specific AI agent */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Runtime Assignment")
	static void AssignActorToAgent(AActor* Agent, FName Key, AActor* TargetActor);

	/** Assign a target actor to all AI agents */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Runtime Assignment", meta = (WorldContext = "WorldContextObject"))
	static void AssignActorToAllAgents(const UObject* WorldContextObject, FName Key, AActor* TargetActor);

	/** Remove an entry from a specific AI agent's world state */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Runtime Assignment")
	static void RemoveEntryFromAgent(AActor* Agent, FName Key);

	// ===== Shared World State =====

	/** Set a bool state that is shared across all AI */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Shared State", meta = (WorldContext = "WorldContextObject"))
	static void SetSharedBoolState(const UObject* WorldContextObject, FName Key, bool Value);

	/** Get a bool state from shared world state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Shared State", meta = (WorldContext = "WorldContextObject"))
	static bool GetSharedBoolState(const UObject* WorldContextObject, FName Key, bool DefaultValue = false);

	/** Set an int state that is shared across all AI */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Shared State", meta = (WorldContext = "WorldContextObject"))
	static void SetSharedIntState(const UObject* WorldContextObject, FName Key, int32 Value);

	/** Get an int state from shared world state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Shared State", meta = (WorldContext = "WorldContextObject"))
	static int32 GetSharedIntState(const UObject* WorldContextObject, FName Key, int32 DefaultValue = 0);

	/** Set a float state that is shared across all AI */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Shared State", meta = (WorldContext = "WorldContextObject"))
	static void SetSharedFloatState(const UObject* WorldContextObject, FName Key, float Value);

	/** Get a float state from shared world state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Shared State", meta = (WorldContext = "WorldContextObject"))
	static float GetSharedFloatState(const UObject* WorldContextObject, FName Key, float DefaultValue = 0.0f);

	// ===== Utility Functions =====

	/** Apply default world knowledge to a specific agent */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Utilities", meta = (WorldContext = "WorldContextObject"))
	static void ApplyDefaultKnowledgeToAgent(const UObject* WorldContextObject, AActor* Agent);

	/** Apply shared state to a specific agent */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Utilities", meta = (WorldContext = "WorldContextObject"))
	static void ApplySharedStateToAgent(const UObject* WorldContextObject, AActor* Agent);

	// ===== Tag-Based Actor Lookup =====

	/** Find the first actor in the world with the specified tag */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup", meta = (WorldContext = "WorldContextObject"))
	static AActor* FindActorByTag(const UObject* WorldContextObject, FName Tag);

	/** Find all actors in the world with the specified tag */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup", meta = (WorldContext = "WorldContextObject"))
	static TArray<AActor*> FindActorsByTag(const UObject* WorldContextObject, FName Tag);

	/** 
	 * Get actor for a given key. First checks assigned actors, then falls back to tag-based lookup.
	 * This is the recommended way to get actors in actions - supports both manual assignment and auto-discovery.
	 */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup", meta = (WorldContext = "WorldContextObject"))
	static AActor* GetActorByKeyOrTag(const UObject* WorldContextObject, AActor* Agent, FName Key);

	/** Refresh the actor cache for improved performance */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup", meta = (WorldContext = "WorldContextObject"))
	static void RefreshActorCache(const UObject* WorldContextObject);
};
