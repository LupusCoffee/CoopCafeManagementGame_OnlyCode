#include "AIManagerRpcBridgeComponent.h"

#include "AIManager.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UAIManagerRpcBridgeComponent::UAIManagerRpcBridgeComponent()
{
	SetIsReplicatedByDefault(true);
}

UAIManagerRpcBridgeComponent* UAIManagerRpcBridgeComponent::FindForWorld(const UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(World, 0);
	if (LocalPC)
	{
		if (UAIManagerRpcBridgeComponent* Bridge = LocalPC->FindComponentByClass<UAIManagerRpcBridgeComponent>())
		{
			return Bridge;
		}

		if (APawn* LocalPawn = LocalPC->GetPawn())
		{
			if (UAIManagerRpcBridgeComponent* Bridge = LocalPawn->FindComponentByClass<UAIManagerRpcBridgeComponent>())
			{
				return Bridge;
			}
		}
	}

	return nullptr;
}

UAIManager* UAIManagerRpcBridgeComponent::GetAIManager() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UAIManager>() : nullptr;
}

void UAIManagerRpcBridgeComponent::Server_RegisterAI_Implementation(APawn* Pawn, const FString& DebugName)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->RegisterAI(Pawn, DebugName);
	}
}

void UAIManagerRpcBridgeComponent::Server_UnregisterAI_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->UnregisterAI(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_SpawnAIFromPool_Implementation(const FString& PoolName, FVector Location, FRotator Rotation)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->SpawnAIFromPool(PoolName, Location, Rotation);
	}
}

void UAIManagerRpcBridgeComponent::Server_ReturnAIToPool_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->ReturnAIToPool(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_PreSpawnPool_Implementation(const FString& PoolName, int32 Count)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->PreSpawnPool(PoolName, Count);
	}
}

void UAIManagerRpcBridgeComponent::Server_ClearPool_Implementation(const FString& PoolName)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->ClearPool(PoolName);
	}
}

void UAIManagerRpcBridgeComponent::Server_ActivateAI_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->ActivateAI(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_DeactivateAI_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DeactivateAI(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_ActivateAllAI_Implementation()
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->ActivateAllAI();
	}
}

void UAIManagerRpcBridgeComponent::Server_DeactivateAllAI_Implementation()
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DeactivateAllAI();
	}
}

void UAIManagerRpcBridgeComponent::Server_ToggleAI_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->ToggleAI(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_ForceAIState_Implementation(APawn* Pawn, EAIManagerState NewState)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->ForceAIState(Pawn, NewState);
	}
}

void UAIManagerRpcBridgeComponent::Server_ForceAllAIState_Implementation(EAIManagerState NewState)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->ForceAllAIState(NewState);
	}
}

void UAIManagerRpcBridgeComponent::Server_RestoreAINormalState_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->RestoreAINormalState(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_RestoreAllAINormalState_Implementation()
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->RestoreAllAINormalState();
	}
}

void UAIManagerRpcBridgeComponent::Server_DebugTeleportAI_Implementation(APawn* Pawn, FVector Location, FRotator Rotation)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DebugTeleportAI(Pawn, Location, Rotation);
	}
}

void UAIManagerRpcBridgeComponent::Server_DebugTeleportAllAI_Implementation(FVector CenterLocation, float Radius)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DebugTeleportAllAI(CenterLocation, Radius);
	}
}

void UAIManagerRpcBridgeComponent::Server_DebugMoveAITo_Implementation(APawn* Pawn, FVector Location)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DebugMoveAITo(Pawn, Location);
	}
}

void UAIManagerRpcBridgeComponent::Server_DebugKillAI_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DebugKillAI(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_DebugResetAI_Implementation(APawn* Pawn)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DebugResetAI(Pawn);
	}
}

void UAIManagerRpcBridgeComponent::Server_DebugSetVisualization_Implementation(APawn* Pawn, bool bEnabled)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DebugSetVisualization(Pawn, bEnabled);
	}
}

void UAIManagerRpcBridgeComponent::Server_DebugHighlightAI_Implementation(APawn* Pawn, float Duration, FColor Color)
{
	if (UAIManager* Manager = GetAIManager())
	{
		Manager->DebugHighlightAI(Pawn, Duration, Color);
	}
}

