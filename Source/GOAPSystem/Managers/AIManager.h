// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/ManagerHelperStructs.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIManager.generated.h"

USTRUCT(BlueprintType)
struct FGoapWorldSubsystemConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "GOAP")
	TMap<FName, TSoftObjectPtr<AActor>> DefaultActors;

	UPROPERTY(EditDefaultsOnly, Category = "GOAP")
	TMap<FName, FVector> DefaultLocations;

	UPROPERTY(EditDefaultsOnly, Category = "GOAP")
	bool bAutoDistributeKnowledge = true;
};

/**
 * 
 */
UCLASS()
class GOAPSYSTEM_API UAIManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	//---------------------API FUNCTIONS---------------------//
	//System lifecycle
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	//Registration
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Registration")
	void RegisterAI(APawn* Pawn, const FString& DebugName = "");
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Registration")
	void UnregisterAI(APawn* Pawn);
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Registration")
	bool IsAIRegistered(APawn* Pawn) const;
	
	//Object pool
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Pool")
	APawn* SpawnAIFromPool(const FString& PoolName, FVector Location, FRotator Rotation);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Pool")
	void ReturnAIToPool(APawn* Pawn);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Pool")
	void PreSpawnPool(const FString& PoolName, int32 Count);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Pool")
	void ClearPool(const FString& PoolName);	
	
	//Activation control
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Activation")
	void ActivateAI(APawn* Pawn);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Activation")
	void DeactivateAI(APawn* Pawn);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Activation")
	void ActivateAllAI();
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Activation")
	void DeactivateAllAI();
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Activation")
	void ToggleAI(APawn* Pawn);
	
	//State management
	UFUNCTION(BlueprintCallable, Category = "AI Manager|State")
	void ForceAIState(APawn* Pawn, EAIManagerState NewState);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|State")
	void ForceAllAIState(EAIManagerState NewState);

	UFUNCTION(BlueprintCallable, Category = "AI Manager|State")
	void RestoreAINormalState(APawn* Pawn);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|State")
	void RestoreAllAINormalState();
	
	//---------------------DEBUG FUNCTIONS---------------------//
	
	//Teleportation
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugTeleportAI(APawn* Pawn, FVector Location, FRotator Rotation = FRotator::ZeroRotator);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugTeleportAllAI(FVector CenterLocation, float Radius = 500.0f);
	
	//Freeze
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugFreezeAI(APawn* Pawn);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugUnfreezeAI(APawn* Pawn);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugFreezeAllAI();
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugUnfreezeAllAI();
	
	//Debug Control
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugMoveAITo(APawn* Pawn, FVector Location);
    
	/** Kill an AI (return to pool) */
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugKillAI(APawn* Pawn);
    
	/** Reset AI to initial state */
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugResetAI(APawn* Pawn);
	
	//Visualization
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugShowAIInfo(APawn* Pawn, bool bShowOnScreen = true);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugShowAllAIInfo(bool bShowOnScreen = true);
    
	/** Enable/disable visualization for an AI */
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugSetVisualization(APawn* Pawn, bool bEnabled);
	
	UFUNCTION(BlueprintCallable, Category = "AI Manager|Debug")
	void DebugHighlightAI(APawn* Pawn, float Duration = 5.0f, FColor Color = FColor::Yellow);
	
	//---------------------END DEBUG FUNCTIONS---------------------//
	
	//---------------------QUERY FUNCTIONS---------------------//
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	TArray<APawn*> GetAllAIPawns() const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	TArray<APawn*> GetActiveAIPawns() const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	TArray<APawn*> GetInactiveAIPawns() const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	bool GetAIInfo(APawn* Pawn, FAIAgentInfo& OutInfo) const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	int32 GetAICount() const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	int32 GetActiveAICount() const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	APawn* FindClosestAI(FVector Location, bool bOnlyActive = true) const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	TArray<APawn*> FindAIInRadius(FVector Location, float Radius, bool bOnlyActive = true) const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	TArray<APawn*> FindAIByState(EAIManagerState State) const;
	
	UFUNCTION(BlueprintPure, Category = "AI Manager|Query")
	APawn* FindAIByName(const FString& DebugName) const;
	
protected:
	//---------------------CONFIGURATION---------------------//
	// Note: Configuration moved to UAIManagerSettings (Edit -> Project Settings -> Plugins -> AI Manager)
	
private:
	//---------------------INTERNALS---------------------//
	//Data
	TMap<TWeakObjectPtr<APawn>, FAIAgentInfo> RegisteredAI;
	
	TMap<FString, TArray<FAIPoolEntry>> ObjectPools;
	
	//Event Handlers (for lifecycle events)
	void OnAISpawnedEvent(APawn* Pawn, const FString& DebugName);
	void OnAIDestroyedEvent(APawn* Pawn);
	void OnAIActivatedEvent(APawn* Pawn);
	void OnAIDeactivatedEvent(APawn* Pawn);
	
	//Methods
	void ApplyStateToAI(FAIAgentInfo& AgentInfo, EAIManagerState NewState);
	
	void InternalActivateAI(FAIAgentInfo& AgentInfo);
	
	void InternalDeactivateAI(FAIAgentInfo& AgentInfo);
	
	FAIPoolEntry* FindAvailablePoolEntry(const FString& PoolName);
	
	FAIPoolEntry* FindPoolEntryForPawn(APawn* Pawn);

	void InitializePoolEntry(FAIPoolEntry& PoolEntry, TSubclassOf<ACharacter> CharacterClass, 
							TSubclassOf<AAIController> ControllerClass, const FString& PoolName);
	
	const FPoolConfig* FindPoolConfig(const FString& PoolName) const;
	
	FString BuildInfoString(const FAIAgentInfo& AgentInfo) const;
	
	void CleanupInvalidEntries();
};
