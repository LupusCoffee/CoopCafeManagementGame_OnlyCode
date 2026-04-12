#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/ManagerHelperStructs.h"
#include "AIManagerRpcBridgeComponent.generated.h"

class UAIManager;
class APawn;
class UWorld;

/**
 * Client-owned RPC relay for UAIManager server-authoritative actions.
 * Add this to a player-owned replicated actor (PlayerController or Pawn).
 */
UCLASS(ClassGroup=(Network), meta=(BlueprintSpawnableComponent))
class GOAPSYSTEM_API UAIManagerRpcBridgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIManagerRpcBridgeComponent();

	UFUNCTION(BlueprintPure, Category = "AI Manager|Network")
	static UAIManagerRpcBridgeComponent* FindForWorld(const UWorld* World);

	UFUNCTION(Server, Reliable)
	void Server_RegisterAI(APawn* Pawn, const FString& DebugName);

	UFUNCTION(Server, Reliable)
	void Server_UnregisterAI(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_SpawnAIFromPool(const FString& PoolName, FVector Location, FRotator Rotation);

	UFUNCTION(Server, Reliable)
	void Server_ReturnAIToPool(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_PreSpawnPool(const FString& PoolName, int32 Count);

	UFUNCTION(Server, Reliable)
	void Server_ClearPool(const FString& PoolName);

	UFUNCTION(Server, Reliable)
	void Server_ActivateAI(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_DeactivateAI(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_ActivateAllAI();

	UFUNCTION(Server, Reliable)
	void Server_DeactivateAllAI();

	UFUNCTION(Server, Reliable)
	void Server_ToggleAI(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_ForceAIState(APawn* Pawn, EAIManagerState NewState);

	UFUNCTION(Server, Reliable)
	void Server_ForceAllAIState(EAIManagerState NewState);

	UFUNCTION(Server, Reliable)
	void Server_RestoreAINormalState(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_RestoreAllAINormalState();

	UFUNCTION(Server, Reliable)
	void Server_DebugTeleportAI(APawn* Pawn, FVector Location, FRotator Rotation);

	UFUNCTION(Server, Reliable)
	void Server_DebugTeleportAllAI(FVector CenterLocation, float Radius);

	UFUNCTION(Server, Reliable)
	void Server_DebugMoveAITo(APawn* Pawn, FVector Location);

	UFUNCTION(Server, Reliable)
	void Server_DebugKillAI(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_DebugResetAI(APawn* Pawn);

	UFUNCTION(Server, Reliable)
	void Server_DebugSetVisualization(APawn* Pawn, bool bEnabled);

	UFUNCTION(Server, Reliable)
	void Server_DebugHighlightAI(APawn* Pawn, float Duration, FColor Color);

private:
	UAIManager* GetAIManager() const;
};


