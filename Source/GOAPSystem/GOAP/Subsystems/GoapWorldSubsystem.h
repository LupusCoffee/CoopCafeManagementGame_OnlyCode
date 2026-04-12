// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GoapWorldState.h"
#include "GoapWorldSubsystem.generated.h"

/**
 * Struct to define default world knowledge entries
 */
USTRUCT(BlueprintType)
struct FGoapDefaultWorldEntry
{
	GENERATED_BODY()

	/** Key name for this world state entry */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	FName Key;

	/** Optional actor reference (can be set at runtime or in editor) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	TSoftObjectPtr<AActor> Actor;

	/** Optional fixed location (alternative to actor) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	FVector Location;

	/** Whether to use the fixed location instead of an actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	bool bUseFixedLocation = false;

	/** Whether this entry should be applied to all AI agents automatically */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	bool bIsGlobalKnowledge = true;

	FGoapDefaultWorldEntry()
		: Key(NAME_None)
		, Location(FVector::ZeroVector)
		, bUseFixedLocation(false)
		, bIsGlobalKnowledge(true)
	{}
};

/**
 * World subsystem that manages shared GOAP world state and default actor references
 * that all AI agents should know about.
 */
UCLASS()
class GOAPSYSTEM_API UGoapWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem lifecycle
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// ===== Default World Knowledge Management =====

	/** Add or update a default actor reference that all AI should know about */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void SetDefaultActor(FName Key, AActor* Actor, bool bIsGlobalKnowledge = true);

	/** Add or update a default location that all AI should know about */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void SetDefaultLocation(FName Key, FVector Location, bool bIsGlobalKnowledge = true);

	/** Remove a default world knowledge entry */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void RemoveDefaultEntry(FName Key);

	/** Get the actor associated with a key from default knowledge */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|World State")
	AActor* GetDefaultActor(FName Key) const;

	/** Get the location associated with a key from default knowledge */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|World State")
	FVector GetDefaultLocation(FName Key, bool& bIsValid) const;

	/** Check if a default entry exists */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|World State")
	bool HasDefaultEntry(FName Key) const;

	/** Get all default world knowledge entries */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|World State")
	const TArray<FGoapDefaultWorldEntry>& GetAllDefaultEntries() const { return DefaultWorldEntries; }

	// ===== Runtime Actor Assignment =====

	/** Assign a target actor to a specific AI agent's world state */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void AssignActorToAgent(AActor* Agent, FName Key, AActor* TargetActor);

	/** Assign a location to a specific AI agent's world state */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void AssignLocationToAgent(AActor* Agent, FName Key, FVector Location);

	/** Remove an entry from a specific AI agent's world state */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void RemoveEntryFromAgent(AActor* Agent, FName Key);

	/** Assign an actor to all registered AI agents */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void AssignActorToAllAgents(FName Key, AActor* TargetActor);

	/** Assign a location to all registered AI agents */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void AssignLocationToAllAgents(FName Key, FVector Location);

	// ===== Shared World State =====

	/** Set a bool state that is shared across all AI */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Shared State")
	void SetSharedBoolState(FName Key, bool Value);

	/** Get a bool state from shared world state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Shared State")
	bool GetSharedBoolState(FName Key, bool DefaultValue = false) const;

	/** Set an int state that is shared across all AI */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Shared State")
	void SetSharedIntState(FName Key, int32 Value);

	/** Get an int state from shared world state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Shared State")
	int32 GetSharedIntState(FName Key, int32 DefaultValue = 0) const;

	/** Set a float state that is shared across all AI */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Shared State")
	void SetSharedFloatState(FName Key, float Value);

	/** Get a float state from shared world state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Shared State")
	float GetSharedFloatState(FName Key, float DefaultValue = 0.0f) const;

	/** Apply the shared world state to a specific agent's world state */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Shared State")
	void ApplySharedStateToAgent(AActor* Agent);

	/** Apply all default knowledge to a specific agent */
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void ApplyDefaultKnowledgeToAgent(AActor* Agent);

	// ===== Query Functions =====

	/** Find all actors registered under a specific key across all agents */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Query")
	TArray<AActor*> FindAllActorsForKey(FName Key) const;

	/** Count how many agents have a specific actor assigned */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Query")
	int32 CountAgentsWithActor(FName Key, AActor* Actor) const;

	// ===== Tag-Based Actor Lookup =====

	/** Find the first actor in the world with the specified tag */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup")
	AActor* FindActorByTag(FName Tag) const;

	/** Find all actors in the world with the specified tag */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup")
	TArray<AActor*> FindActorsByTag(FName Tag) const;

	/** 
	 * Get actor for a given key. First checks assigned actors in world state,
	 * then falls back to tag-based lookup if no actor is assigned.
	 * This allows seamless integration between manual assignment and automatic tag discovery.
	 */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup")
	AActor* GetActorByKeyOrTag(AActor* Agent, FName Key) const;

	/** Refresh the actor cache for improved performance */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Actor Lookup")
	void RefreshActorCache();

protected:
	/** Default world knowledge entries configured in editor or at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Configuration")
	TArray<FGoapDefaultWorldEntry> DefaultWorldEntries;

	/** Shared world state that can be accessed by all AI */
	UPROPERTY(BlueprintReadOnly, Category = "GOAP|State")
	FGoapWorldState SharedWorldState;

	/** Whether to automatically apply default knowledge to new agents */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Configuration")
	bool bAutoApplyDefaultKnowledge = true;

private:
	/** Helper to get GOAP component from an actor */
	class UGoapComponent* GetGoapComponent(AActor* Agent) const;

	/** Initialize default world knowledge entries */
	void InitializeDefaultEntries();

	/** Cache for tag-based actor lookups to improve performance */
	UPROPERTY()
	TMap<FName, TObjectPtr<AActor>> CachedActorsByTag;
};
