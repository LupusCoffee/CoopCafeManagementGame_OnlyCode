#pragma once
#include "AIController.h"
#include "GOAPSystem/AI/Components/ActivatableComponent.h"
#include "GOAPSystem/GOAP/Components/GoapComponent.h"
#include "ManagerHelperStructs.generated.h"

UENUM(BlueprintType)
enum class EAIManagerState : uint8
{
	Normal          UMETA(DisplayName = "Normal"),          // AI behaves normally
	Paused          UMETA(DisplayName = "Paused"),          // AI logic paused, frozen in place
	Disabled        UMETA(DisplayName = "Disabled"),        // AI completely disabled
	DebugControlled UMETA(DisplayName = "Debug Controlled") // Under debug control
};


//Stores metadata about each registered AI agent for tracking and management.
USTRUCT(BlueprintType)
struct FAIAgentInfo
{
	GENERATED_BODY()

	// Core references
	UPROPERTY()
	TWeakObjectPtr<APawn> Pawn;
    
	UPROPERTY()
	TWeakObjectPtr<AAIController> Controller;
    
	UPROPERTY()
	TWeakObjectPtr<UGoapComponent> GoapComponent;
    
	UPROPERTY()
	TWeakObjectPtr<UActivatableComponent> ActivatableComponent;
    
	// State tracking
	UPROPERTY()
	EAIManagerState CurrentState = EAIManagerState::Normal;
    
	UPROPERTY()
	bool bIsActive = true;
    
	// Spawn information
	UPROPERTY()
	FVector SpawnLocation = FVector::ZeroVector;
    
	UPROPERTY()
	FRotator SpawnRotation = FRotator::ZeroRotator;
    
	// Debug identification
	UPROPERTY()
	FString DebugName;
    
	// Registration time
	UPROPERTY()
	float RegistrationTime = 0.0f;
};

//Configuration settings for AI character pools.
USTRUCT(BlueprintType)
struct FPoolConfig
{
	GENERATED_BODY()

	// What to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Pool Config")
	TSubclassOf<ACharacter> CharacterClass;
    
	UPROPERTY(EditDefaultsOnly, Category = "Pool Config")
	TSubclassOf<AAIController> ControllerClass;
    
	// Pool sizing
	UPROPERTY(EditDefaultsOnly, Category = "Pool Config")
	int32 PreSpawnCount = 0;
    
	UPROPERTY(EditDefaultsOnly, Category = "Pool Config")
	int32 MaxPoolSize = 50;
    
	// Identification
	UPROPERTY(EditDefaultsOnly, Category = "Pool Config")
	FString PoolName;
};

//Single pool entry information
USTRUCT(BlueprintType)
struct FAIPoolEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<ACharacter> Character;
    
	UPROPERTY()
	TWeakObjectPtr<AAIController> Controller;
    
	UPROPERTY()
	bool bInUse = false;
    
	UPROPERTY()
	FString PoolName; // Track which pool this entry belongs to
    
	UPROPERTY()
	FVector OriginalSpawnLocation = FVector::ZeroVector;
    
	UPROPERTY()
	FRotator OriginalSpawnRotation = FRotator::ZeroRotator;
    
	UPROPERTY()
	float TimeReturned = 0.0f; // For debugging
    
	UPROPERTY()
	int32 TimesUsed = 0; // Statistics
};