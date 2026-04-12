// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "AIManagerSettings.h"
#include "AIManagerRpcBridgeComponent.h"
#include "BrainComponent.h"
#include "GOAPSystem/AI/Character/BaseAICharacter.h"
#include "GOAPSystem/AI/Events/AILifecycleEvents.h"
#include "GOAPSystem/AI/Components/AIDebugComponent.h"
#include "GOAPSystem/AI/Utils/AIComponentUtils.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(CoffeeNet, Log, All);

namespace
{
	bool IsAuthorityWorld(const UWorld* World)
	{
		return World && World->GetNetMode() != NM_Client;
	}

	bool EnsureAuthority(const UWorld* World, const TCHAR* FunctionName)
	{
		if (IsAuthorityWorld(World))
		{
			return true;
		}

		UE_LOG(CoffeeNet, Verbose, TEXT("AIManager: Ignoring '%s' on client (server-authority only)"), FunctionName);
		return false;
	}

	template <typename TForwardFn>
	bool ForwardToAuthorityIfClient(UWorld* World, const TCHAR* FunctionName, TForwardFn&& ForwardFn)
	{
		if (IsAuthorityWorld(World))
		{
			return false;
		}

		if (UAIManagerRpcBridgeComponent* Bridge = UAIManagerRpcBridgeComponent::FindForWorld(World))
		{
			ForwardFn(Bridge);
			UE_LOG(CoffeeNet, Verbose, TEXT("AIManager: Forwarded '%s' request to authority"), FunctionName);
		}
		else
		{
			UE_LOG(CoffeeNet, Warning,
				TEXT("AIManager: Client call '%s' has no RPC bridge. Add UAIManagerRpcBridgeComponent to a player-owned replicated actor."),
				FunctionName);
		}

		return true;
	}
}

void UAIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Load settings
	const UAIManagerSettings* Settings = UAIManagerSettings::Get();
	if (Settings)
	{
		UE_LOG(CoffeeNet, Log, TEXT("AIManager: Initializing with settings..."));
		UE_LOG(CoffeeNet, Log, TEXT("  - Auto Pre-Spawn: %s"), Settings->bAutoPreSpawn ? TEXT("Enabled") : TEXT("Disabled"));
		UE_LOG(CoffeeNet, Log, TEXT("  - Global Debug Draw: %s"), Settings->bGlobalDebugDraw ? TEXT("Enabled") : TEXT("Disabled"));
		UE_LOG(CoffeeNet, Log, TEXT("  - Default Pools: %d"), Settings->DefaultPools.Num());
		UE_LOG(CoffeeNet, Log, TEXT("  - Max Planning Nodes: %d"), Settings->MaxPlanningNodes);
		UE_LOG(CoffeeNet, Log, TEXT("  - Replan On Failure: %s"), Settings->bReplanOnFailure ? TEXT("Enabled") : TEXT("Disabled"));
	}
	else
	{
		UE_LOG(CoffeeNet, Warning, TEXT("AIManager: Failed to load settings, using defaults"));
	}
}

void UAIManager::Deinitialize()
{
	// Unsubscribe from events
	if (UWorld* World = GetWorld())
	{
		if (UAILifecycleEvents* Events = UAILifecycleEvents::Get(World))
		{
			Events->OnAISpawned.RemoveAll(this);
			Events->OnAIDestroyed.RemoveAll(this);
			Events->OnAIActivated.RemoveAll(this);
			Events->OnAIDeactivated.RemoveAll(this);
		}
	}
	
	// Clean up all registered AI
	RegisteredAI.Empty();
	
	// Clean up pools
	if (IsAuthorityWorld(GetWorld()))
	{
		for (auto& PoolPair : ObjectPools)
		{
			for (FAIPoolEntry& Entry : PoolPair.Value)
			{
				if (ACharacter* Character = Entry.Character.Get())
				{
					Character->Destroy();
				}
			}
		}
	}
	ObjectPools.Empty();
	
	UE_LOG(CoffeeNet, Log, TEXT("AIManager: Deinitialized"));
	
	Super::Deinitialize();
}

void UAIManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (!IsAuthorityWorld(&InWorld))
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("AIManager: Client instance initialized in read-only mode"));
		return;
	}
	
	// Subscribe to AI lifecycle events
	if (UAILifecycleEvents* Events = UAILifecycleEvents::Get(&InWorld))
	{
		Events->OnAISpawned.AddUObject(this, &UAIManager::OnAISpawnedEvent);
		Events->OnAIDestroyed.AddUObject(this, &UAIManager::OnAIDestroyedEvent);
		Events->OnAIActivated.AddUObject(this, &UAIManager::OnAIActivatedEvent);
		Events->OnAIDeactivated.AddUObject(this, &UAIManager::OnAIDeactivatedEvent);
		
		UE_LOG(CoffeeNet, Log, TEXT("AIManager: Subscribed to AI lifecycle events"));
	}
	
	// Load settings and auto pre-spawn pools if configured
	const UAIManagerSettings* Settings = UAIManagerSettings::Get();
	if (Settings && Settings->bAutoPreSpawn)
	{
		UE_LOG(CoffeeNet, Log, TEXT("AIManager: Auto pre-spawning %d pools..."), Settings->DefaultPools.Num());
		for (const FPoolConfig& Config : Settings->DefaultPools)
		{
			if (Config.PreSpawnCount > 0)
			{
				PreSpawnPool(Config.PoolName, Config.PreSpawnCount);
				UE_LOG(CoffeeNet, Log, TEXT("  - Pre-spawned %d AI for pool '%s'"), Config.PreSpawnCount, *Config.PoolName);
			}
		}
	}
}

void UAIManager::RegisterAI(APawn* Pawn, const FString& DebugName)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("RegisterAI"),
		[Pawn, DebugName](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_RegisterAI(Pawn, DebugName);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Cannot register null pawn"));
		return;
	}
	
	TWeakObjectPtr<APawn> WeakPawn(Pawn);
	
	if (RegisteredAI.Contains(WeakPawn))
	{
		UE_LOG(CoffeeNet, Warning, TEXT("AI already registered: %s"), *Pawn->GetName());
		return;
	}
	
	FAIAgentInfo AgentInfo;
	AgentInfo.Pawn = WeakPawn;
	AgentInfo.Controller = Cast<AAIController>(Pawn->GetController());
	AgentInfo.DebugName = DebugName.IsEmpty() ? Pawn->GetName() : DebugName;
	AgentInfo.RegistrationTime = GetWorld()->GetTimeSeconds();
	AgentInfo.CurrentState = EAIManagerState::Normal;
	AgentInfo.bIsActive = true;
	
	RegisteredAI.Add(WeakPawn, AgentInfo);
	
	UE_LOG(CoffeeNet, Log, TEXT("Registered AI: %s"), *AgentInfo.DebugName);
}

void UAIManager::UnregisterAI(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("UnregisterAI"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_UnregisterAI(Pawn);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
	
	TWeakObjectPtr<APawn> WeakPawn(Pawn);
	
	if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		UE_LOG(CoffeeNet, Log, TEXT("Unregistered AI: %s"), *AgentInfo->DebugName);
		RegisteredAI.Remove(WeakPawn);
	}
}

bool UAIManager::IsAIRegistered(APawn* Pawn) const
{
	auto Agent = RegisteredAI.Find(Pawn);
	
	if (!Agent) return false;
	
	return true;
}

APawn* UAIManager::SpawnAIFromPool(const FString& PoolName, FVector Location, FRotator Rotation)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("SpawnAIFromPool"),
		[PoolName, Location, Rotation](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_SpawnAIFromPool(PoolName, Location, Rotation);
		}))
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("AIManager: SpawnAIFromPool is async over RPC on clients and returns nullptr immediately"));
		return nullptr;
	}
	
	TArray<FAIPoolEntry>* Pool = ObjectPools.Find(PoolName);
	
	if (!Pool)
	{
		UE_LOG(CoffeeNet, Error, TEXT("Pool not found: %s"), *PoolName);
		return nullptr;
	}
	
	FAIPoolEntry* PoolEntry = FindAvailablePoolEntry(PoolName);
	
	if (!PoolEntry)
	{
		const FPoolConfig* Config = FindPoolConfig(PoolName);
        
		if (Config && Pool->Num() < Config->MaxPoolSize)
		{
			FAIPoolEntry NewEntry;
			InitializePoolEntry(NewEntry, Config->CharacterClass, Config->ControllerClass, PoolName);
			Pool->Add(NewEntry);
			PoolEntry = &(*Pool)[Pool->Num() - 1];
		}
		else
		{
			UE_LOG(CoffeeNet, Warning, TEXT("Pool exhausted and at max size: %s"), *PoolName);
			return nullptr;
		}
	}

	ACharacter* Character = PoolEntry->Character.Get();
	
	if (!Character)
	{
		UE_LOG(CoffeeNet, Error, TEXT("Invalid character in pool entry"));
		return nullptr;
	}
	
	PoolEntry->bInUse = true;
	PoolEntry->TimesUsed++;
	
	// Use ActivatableComponent's ActivateAtLocation method to handle everything
	if (UActivatableComponent* ActivatableComp = FAIComponentUtils::GetActivatableComponent(Character))
	{
		ActivatableComp->ActivateAtLocation(Location, Rotation);
	}
	else
	{
		// Fallback if no ActivatableComponent
		Character->SetActorLocation(Location);
		Character->SetActorRotation(Rotation);
	}
	
	if (ABaseAICharacter* BaseAI = Cast<ABaseAICharacter>(Character))
	{
		BaseAI->OnPoolSpawned(Location, Rotation);
	}
	
	RegisterAI(Character, FString::Printf(TEXT("%s_%d"), *PoolName, PoolEntry->TimesUsed));
    
	return Character;
}

void UAIManager::ReturnAIToPool(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("ReturnAIToPool"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_ReturnAIToPool(Pawn);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
	
	FAIPoolEntry* PoolEntry = FindPoolEntryForPawn(Pawn);
	if (!PoolEntry)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Pawn not from pool: %s"), *Pawn->GetName());
		return;
	}
	
	UnregisterAI(Pawn);

	// Ensure pooled AI never carries stale GOAP plans/state into the next spawn cycle.
	FAIComponentUtils::ResetGoapExecution(Pawn);
	
	if (ABaseAICharacter* BaseAI = Cast<ABaseAICharacter>(Pawn))
	{
		BaseAI->OnPoolReturned();
	}
	
	// Get hidden location from settings
	const UAIManagerSettings* Settings = UAIManagerSettings::Get();
	FVector HiddenLocation = Settings ? Settings->PoolHiddenLocation : FVector(0, 0, -10000);
	
	// Use ActivatableComponent's DeactivateAndMoveToHiddenLocation method
	// This now handles: stopping AI logic, stopping movement, stopping brain, requesting GOAP replan, etc.
	if (UActivatableComponent* ActivatableComp = FAIComponentUtils::GetActivatableComponent(Pawn))
	{
		ActivatableComp->DeactivateAndMoveToHiddenLocation(HiddenLocation);
	}
	else
	{
		// Fallback if no ActivatableComponent - minimal deactivation
		Pawn->SetActorLocation(HiddenLocation);
		Pawn->SetActorRotation(FRotator::ZeroRotator);
		Pawn->SetActorTickEnabled(false);
		Pawn->SetActorHiddenInGame(true);
		Pawn->SetActorEnableCollision(false);
	}
	
	PoolEntry->bInUse = false;
	PoolEntry->TimeReturned = GetWorld()->GetTimeSeconds();
    
	UE_LOG(CoffeeNet, Log, TEXT("Returned AI to pool '%s': %s"), *PoolEntry->PoolName, *Pawn->GetName());
}

void UAIManager::PreSpawnPool(const FString& PoolName, int32 Count)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("PreSpawnPool"),
		[PoolName, Count](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_PreSpawnPool(PoolName, Count);
		}))
	{
		return;
	}

	const FPoolConfig* Config = FindPoolConfig(PoolName);
    
	if (!Config)
	{
		UE_LOG(CoffeeNet, Error, TEXT("Pool config not found: %s"), *PoolName);
		return;
	}
	
	TArray<FAIPoolEntry>& Pool = ObjectPools.FindOrAdd(PoolName);
	
	for (int32 i = 0; i < Count; ++i)
	{
		FAIPoolEntry NewEntry;
		InitializePoolEntry(NewEntry, Config->CharacterClass, Config->ControllerClass, PoolName);
		Pool.Add(NewEntry);
	}
    
	UE_LOG(CoffeeNet, Log, TEXT("Pre-spawned %d AI for pool: %s"), Count, *PoolName);
}

void UAIManager::ClearPool(const FString& PoolName)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("ClearPool"),
		[PoolName](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_ClearPool(PoolName);
		}))
	{
		return;
	}

	const FPoolConfig* Config = FindPoolConfig(PoolName);
	
	if (!Config)
	{
		UE_LOG(CoffeeNet, Error, TEXT("Pool config not found: %s"), *PoolName);
		return;
	}

	if (TArray<FAIPoolEntry>* Pool = ObjectPools.Find(PoolName))
	{
		for (FAIPoolEntry& Entry : *Pool)
		{
			if (ACharacter* Character = Entry.Character.Get())
			{
				Character->Destroy();
			}
		}
		
		ObjectPools.Remove(PoolName);
		
		UE_LOG(CoffeeNet, Log, TEXT("Cleared pool: %s"), *PoolName);
	}
}


void UAIManager::ActivateAI(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("ActivateAI"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_ActivateAI(Pawn);
		}))
	{
		return;
	}

	if (!IsAIRegistered(Pawn))
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Cannot activate unregistered AI"));
		return;
	}
	
	TWeakObjectPtr<APawn> WeakPawn(Pawn);
	if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		InternalActivateAI(*AgentInfo);
	}
}

void UAIManager::DeactivateAI(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DeactivateAI"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DeactivateAI(Pawn);
		}))
	{
		return;
	}

	if (!IsAIRegistered(Pawn))
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Cannot deactivate unregistered AI"));
		return;
	}
	
	TWeakObjectPtr<APawn> WeakPawn(Pawn);
	if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		InternalDeactivateAI(*AgentInfo);
	}
}

void UAIManager::ActivateAllAI()
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("ActivateAllAI"),
		[](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_ActivateAllAI();
		}))
	{
		return;
	}

	for (auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid())
		{
			InternalActivateAI(Pair.Value);
		}
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("Activated all AI"));
}

void UAIManager::DeactivateAllAI()
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DeactivateAllAI"),
		[](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DeactivateAllAI();
		}))
	{
		return;
	}

	for (auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid())
		{
			InternalDeactivateAI(Pair.Value);
		}
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("Deactivated all AI"));
}

void UAIManager::ToggleAI(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("ToggleAI"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_ToggleAI(Pawn);
		}))
	{
		return;
	}

	if (!IsAIRegistered(Pawn))
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Cannot toggle unregistered AI"));
		return;
	}
	
	TWeakObjectPtr<APawn> WeakPawn(Pawn);
	if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		if (AgentInfo->bIsActive)
		{
			InternalDeactivateAI(*AgentInfo);
		}
		else
		{
			InternalActivateAI(*AgentInfo);
		}
	}
}

void UAIManager::ForceAIState(APawn* Pawn, EAIManagerState NewState)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("ForceAIState"),
		[Pawn, NewState](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_ForceAIState(Pawn, NewState);
		}))
	{
		return;
	}

	if (!IsAIRegistered(Pawn))
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Cannot force state on unregistered AI"));
		return;
	}
    
	TWeakObjectPtr<APawn> WeakPawn(Pawn);

	if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		ApplyStateToAI(*AgentInfo, NewState);
	}

}

void UAIManager::ForceAllAIState(EAIManagerState NewState)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("ForceAllAIState"),
		[NewState](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_ForceAllAIState(NewState);
		}))
	{
		return;
	}

	for (auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid())
		{
			ApplyStateToAI(Pair.Value, NewState);
		}
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("Forced all AI to state: %s"), *UEnum::GetValueAsString(NewState));
}

void UAIManager::RestoreAINormalState(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("RestoreAINormalState"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_RestoreAINormalState(Pawn);
		}))
	{
		return;
	}

	if (!IsAIRegistered(Pawn))
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Cannot force state on unregistered AI"));
		return;
	}
    
	TWeakObjectPtr<APawn> WeakPawn(Pawn);

	if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		ApplyStateToAI(*AgentInfo, EAIManagerState::Normal);
	}
}

void UAIManager::RestoreAllAINormalState()
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("RestoreAllAINormalState"),
		[](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_RestoreAllAINormalState();
		}))
	{
		return;
	}

	for (auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid())
		{
			ApplyStateToAI(Pair.Value, EAIManagerState::Normal);
		}
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("Restored all AI to normal state"));
}

void UAIManager::DebugTeleportAI(APawn* Pawn, FVector Location, FRotator Rotation)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DebugTeleportAI"),
		[Pawn, Location, Rotation](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DebugTeleportAI(Pawn, Location, Rotation);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
	
	Pawn->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
    
	if (!Rotation.IsZero())
	{
		Pawn->SetActorRotation(Rotation);
	}
	
	if (AController* Controller = Pawn->GetController())
	{
		if (AAIController* AIController = Cast<AAIController>(Controller))
		{
			AIController->StopMovement();
		}
	}
    
	UE_LOG(CoffeeNet, Log, TEXT("Teleported AI %s to %s"), *Pawn->GetName(), *Location.ToString());
	
	const UAIManagerSettings* Settings = UAIManagerSettings::Get();
	if (Settings && Settings->bGlobalDebugDraw)
	{
		DrawDebugSphere(GetWorld(), Location, 100.0f, 16, FColor::Yellow, false, 3.0f);
	}
}

void UAIManager::DebugTeleportAllAI(FVector CenterLocation, float Radius)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DebugTeleportAllAI"),
		[CenterLocation, Radius](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DebugTeleportAllAI(CenterLocation, Radius);
		}))
	{
		return;
	}

	TArray<APawn*> AllAI = GetAllAIPawns();
	int32 Count = AllAI.Num();
    
	if (Count == 0)
	{
		return;
	}
    
	for (int32 i = 0; i < Count; ++i)
	{
		// Circle teleport
		float Angle = (2.0f * PI * i) / Count;
		FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Radius;
		FVector TargetLocation = CenterLocation + Offset;
        
		DebugTeleportAI(AllAI[i], TargetLocation, FRotator::ZeroRotator);
	}
}

void UAIManager::DebugFreezeAI(APawn* Pawn)
{
	ForceAIState(Pawn, EAIManagerState::Paused);
}

void UAIManager::DebugUnfreezeAI(APawn* Pawn)
{
	RestoreAINormalState(Pawn);
}

void UAIManager::DebugFreezeAllAI()
{
	ForceAllAIState(EAIManagerState::Paused);
}

void UAIManager::DebugUnfreezeAllAI()
{
	RestoreAllAINormalState();
}

void UAIManager::DebugMoveAITo(APawn* Pawn, FVector Location)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DebugMoveAITo"),
		[Pawn, Location](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DebugMoveAITo(Pawn, Location);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
	
	AAIController* AIController = Cast<AAIController>(Pawn->GetController());
	if (!AIController)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("AI has no AIController: %s"), *Pawn->GetName());
		return;
	}
	
	AIController->MoveToLocation(Location);
	
	UE_LOG(CoffeeNet, Log, TEXT("Commanded AI %s to move to %s"), *Pawn->GetName(), *Location.ToString());
	
	const UAIManagerSettings* Settings = UAIManagerSettings::Get();
	if (Settings && Settings->bGlobalDebugDraw)
	{
		DrawDebugSphere(GetWorld(), Location, 50.0f, 12, FColor::Green, false, 5.0f);
		DrawDebugLine(GetWorld(), Pawn->GetActorLocation(), Location, FColor::Green, false, 5.0f, 0, 2.0f);
	}
}

void UAIManager::DebugKillAI(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DebugKillAI"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DebugKillAI(Pawn);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
	
	// Try to return to pool first
	FAIPoolEntry* PoolEntry = FindPoolEntryForPawn(Pawn);
	if (PoolEntry)
	{
		ReturnAIToPool(Pawn);
		UE_LOG(CoffeeNet, Log, TEXT("Killed AI (returned to pool): %s"), *Pawn->GetName());
	}
	else
	{
		// Not from pool, destroy it
		UnregisterAI(Pawn);
		Pawn->Destroy();
		UE_LOG(CoffeeNet, Log, TEXT("Killed AI (destroyed): %s"), *Pawn->GetName());
	}
}

void UAIManager::DebugResetAI(APawn* Pawn)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DebugResetAI"),
		[Pawn](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DebugResetAI(Pawn);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
	
	// Reset state to normal
	RestoreAINormalState(Pawn);
	
	// Stop movement and restart AI logic using utilities
	FAIComponentUtils::StopAIMovement(Pawn);
	FAIComponentUtils::RestartAILogic(Pawn);
	
	// Reset GOAP if present
	FAIComponentUtils::RequestGoapReplan(Pawn);
	
	UE_LOG(CoffeeNet, Log, TEXT("Reset AI: %s"), *Pawn->GetName());
}

void UAIManager::DebugShowAIInfo(APawn* Pawn, bool bShowOnScreen)
{
	if (!Pawn || !IsAIRegistered(Pawn))
	{
		return;
	}
	
	TWeakObjectPtr<APawn> WeakPawn(Pawn);
	if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		FString InfoString = BuildInfoString(*AgentInfo);
		
		if (bShowOnScreen && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, InfoString);
		}
		
		UE_LOG(CoffeeNet, Log, TEXT("%s"), *InfoString);
	}
}

void UAIManager::DebugShowAllAIInfo(bool bShowOnScreen)
{
	if (RegisteredAI.Num() == 0)
	{
		UE_LOG(CoffeeNet, Log, TEXT("No AI registered"));
		return;
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("=== AI Manager Info ==="));
	UE_LOG(CoffeeNet, Log, TEXT("Total AI: %d"), RegisteredAI.Num());
	UE_LOG(CoffeeNet, Log, TEXT("Active AI: %d"), GetActiveAICount());
	
	for (const auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid())
		{
			FString InfoString = BuildInfoString(Pair.Value);
			UE_LOG(CoffeeNet, Log, TEXT("%s"), *InfoString);
			
			if (bShowOnScreen && GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, InfoString);
			}
		}
	}
}

void UAIManager::DebugSetVisualization(APawn* Pawn, bool bEnabled)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DebugSetVisualization"),
		[Pawn, bEnabled](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DebugSetVisualization(Pawn, bEnabled);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
	
	// Find AIDebugComponent using utility
	if (UAIDebugComponent* DebugComp = FAIComponentUtils::GetDebugComponent(Pawn))
	{
		DebugComp->SetVisualizationEnabled(bEnabled);
		UE_LOG(CoffeeNet, Log, TEXT("Set visualization %s for AI: %s"), 
			bEnabled ? TEXT("ON") : TEXT("OFF"), *Pawn->GetName());
	}
	else
	{
		UE_LOG(CoffeeNet, Warning, TEXT("AI has no UAIDebugComponent: %s"), *Pawn->GetName());
	}
}

void UAIManager::DebugHighlightAI(APawn* Pawn, float Duration, FColor Color)
{
	if (ForwardToAuthorityIfClient(GetWorld(), TEXT("DebugHighlightAI"),
		[Pawn, Duration, Color](UAIManagerRpcBridgeComponent* Bridge)
		{
			Bridge->Server_DebugHighlightAI(Pawn, Duration, Color);
		}))
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}
    
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
    
	FVector Location = Pawn->GetActorLocation();
	float Height = Pawn->GetSimpleCollisionCylinderExtent().Z;

	DrawDebugSphere(World, Location, 100.0f, 16, Color, false, Duration, 0, 3.0f);
	
	DrawDebugLine(World, Location, Location + FVector(0, 0, Height + 200),
				  Color, false, Duration, 0, 5.0f);

	DrawDebugString(World, Location + FVector(0, 0, Height + 220),
				   Pawn->GetName(), nullptr, Color, Duration, true);
}

TArray<APawn*> UAIManager::GetAllAIPawns() const
{
	TArray<APawn*> Result;
	
	for (const auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid())
		{
			Result.Add(Pair.Key.Get());
		}
	}
	
	return Result;
}

TArray<APawn*> UAIManager::GetActiveAIPawns() const
{
	TArray<APawn*> Result;
	
	for (const auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid() && Pair.Value.bIsActive)
		{
			Result.Add(Pair.Key.Get());
		}
	}
	
	return Result;
}

TArray<APawn*> UAIManager::GetInactiveAIPawns() const
{
	TArray<APawn*> Result;
	
	for (const auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid() && !Pair.Value.bIsActive)
		{
			Result.Add(Pair.Key.Get());
		}
	}
	
	return Result;
}

bool UAIManager::GetAIInfo(APawn* Pawn, FAIAgentInfo& OutInfo) const
{
	if (!Pawn)
	{
		return false;
	}
	
	TWeakObjectPtr<APawn> WeakPawn(Pawn);
	if (const FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
	{
		OutInfo = *AgentInfo;
		return true;
	}
	
	return false;
}

int32 UAIManager::GetAICount() const
{
	return RegisteredAI.Num();
}

int32 UAIManager::GetActiveAICount() const
{
	int32 Count = 0;
	
	for (const auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid() && Pair.Value.bIsActive)
		{
			Count++;
		}
	}
	
	return Count;
}

APawn* UAIManager::FindClosestAI(FVector Location, bool bOnlyActive) const
{
	APawn* ClosestPawn = nullptr;
	float ClosestDistSq = MAX_FLT;
    
	for (const auto& Pair : RegisteredAI)
	{
		if (!Pair.Key.IsValid())
		{
			continue;
		}
        
		const FAIAgentInfo& AgentInfo = Pair.Value;
        
		if (bOnlyActive && !AgentInfo.bIsActive)
		{
			continue;
		}
        
		APawn* Pawn = Pair.Key.Get();
		float DistSq = FVector::DistSquared(Location, Pawn->GetActorLocation());
        
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestPawn = Pawn;
		}
	}
    
	return ClosestPawn;
}

TArray<APawn*> UAIManager::FindAIInRadius(FVector Location, float Radius, bool bOnlyActive) const
{
	TArray<APawn*> Result;
	float RadiusSq = Radius * Radius;
    
	for (const auto& Pair : RegisteredAI)
	{
		if (!Pair.Key.IsValid())
		{
			continue;
		}
        
		const FAIAgentInfo& AgentInfo = Pair.Value;
        
		if (bOnlyActive && !AgentInfo.bIsActive)
		{
			continue;
		}
        
		APawn* Pawn = Pair.Key.Get();
		float DistSq = FVector::DistSquared(Location, Pawn->GetActorLocation());
        
		if (DistSq <= RadiusSq)
		{
			Result.Add(Pawn);
		}
	}
    
	return Result;
}

TArray<APawn*> UAIManager::FindAIByState(EAIManagerState State) const
{
	TArray<APawn*> Result;
	
	for (const auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid() && Pair.Value.CurrentState == State)
		{
			Result.Add(Pair.Key.Get());
		}
	}
	
	return Result;
}

APawn* UAIManager::FindAIByName(const FString& DebugName) const
{
	for (const auto& Pair : RegisteredAI)
	{
		if (Pair.Key.IsValid() && Pair.Value.DebugName == DebugName)
		{
			return Pair.Key.Get();
		}
	}
	
	return nullptr;
}

void UAIManager::OnAISpawnedEvent(APawn* Pawn, const FString& DebugName)
{
	if (!EnsureAuthority(GetWorld(), TEXT("OnAISpawnedEvent")))
	{
		return;
	}
	RegisterAI(Pawn, DebugName);
}

void UAIManager::OnAIDestroyedEvent(APawn* Pawn)
{
	if (!EnsureAuthority(GetWorld(), TEXT("OnAIDestroyedEvent")))
	{
		return;
	}
	UnregisterAI(Pawn);
}

void UAIManager::OnAIActivatedEvent(APawn* Pawn)
{
	if (!EnsureAuthority(GetWorld(), TEXT("OnAIActivatedEvent")))
	{
		return;
	}

	// Handle AI activation event
	if (IsAIRegistered(Pawn))
	{
		TWeakObjectPtr<APawn> WeakPawn(Pawn);
		if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
		{
			AgentInfo->bIsActive = true;
			UE_LOG(CoffeeNet, Log, TEXT("AIManager: AI activated via event: %s"), *AgentInfo->DebugName);
		}
	}
}

void UAIManager::OnAIDeactivatedEvent(APawn* Pawn)
{
	if (!EnsureAuthority(GetWorld(), TEXT("OnAIDeactivatedEvent")))
	{
		return;
	}

	// Handle AI deactivation event
	if (IsAIRegistered(Pawn))
	{
		TWeakObjectPtr<APawn> WeakPawn(Pawn);
		if (FAIAgentInfo* AgentInfo = RegisteredAI.Find(WeakPawn))
		{
			AgentInfo->bIsActive = false;
			UE_LOG(CoffeeNet, Log, TEXT("AIManager: AI deactivated via event: %s"), *AgentInfo->DebugName);
		}
	}
}

void UAIManager::ApplyStateToAI(FAIAgentInfo& AgentInfo, EAIManagerState NewState)
{
	if (!EnsureAuthority(GetWorld(), TEXT("ApplyStateToAI")))
	{
		return;
	}

	EAIManagerState OldState = AgentInfo.CurrentState;
	AgentInfo.CurrentState = NewState;
	
	APawn* Pawn = AgentInfo.Pawn.Get();
	AAIController* Controller = AgentInfo.Controller.Get();
	UBrainComponent* Brain = Controller ? Controller->GetBrainComponent() : nullptr;
	
	switch (NewState)
	{
		case EAIManagerState::Normal:
			if (Brain)
			{
				Brain->ResumeLogic(TEXT("AIManager"));
			}
			if (Pawn)
			{
				Pawn->SetActorTickEnabled(true);
			}
			if (Controller)
			{
				Controller->SetActorTickEnabled(true);
			}
			break;
			
		case EAIManagerState::Paused:
			if (Brain)
			{
				Brain->PauseLogic(TEXT("AIManager_Paused"));
			}
			if (Controller)
			{
				Controller->StopMovement();
			}
			break;
			
		case EAIManagerState::Disabled:
			if (Brain)
			{
				Brain->StopLogic(TEXT("AIManager_Disabled"));
			}
			if (Controller)
			{
				Controller->StopMovement();
				Controller->SetActorTickEnabled(false);
			}
			if (Pawn)
			{
				Pawn->SetActorTickEnabled(false);
			}
			break;
			
		case EAIManagerState::DebugControlled:
			if (Brain)
			{
				Brain->PauseLogic(TEXT("AIManager_Debug"));
			}
			if (Controller)
			{
				Controller->StopMovement();
			}
			break;
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("AI state changed: %s -> %s (AI: %s)"), 
		*UEnum::GetValueAsString(OldState), 
		*UEnum::GetValueAsString(NewState),
		*AgentInfo.DebugName);
}

void UAIManager::InternalActivateAI(FAIAgentInfo& AgentInfo)
{
	if (!EnsureAuthority(GetWorld(), TEXT("InternalActivateAI")))
	{
		return;
	}

	if (AgentInfo.bIsActive)
	{
		return;
	}
	
	AgentInfo.bIsActive = true;
	ApplyStateToAI(AgentInfo, EAIManagerState::Normal);
	
	APawn* Pawn = AgentInfo.Pawn.Get();
	if (Pawn)
	{
		// Delegate to ActivatableComponent - it handles all the activation logic
		if (UActivatableComponent* ActivatableComp = FAIComponentUtils::GetActivatableComponent(Pawn))
		{
			ActivatableComp->ActivateActor();
		}
		else
		{
			// Fallback if no ActivatableComponent - minimal activation
			Pawn->SetActorTickEnabled(true);
			Pawn->SetActorHiddenInGame(false);
			Pawn->SetActorEnableCollision(true);
		}
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("Activated AI: %s"), *AgentInfo.DebugName);
}

void UAIManager::InternalDeactivateAI(FAIAgentInfo& AgentInfo)
{
	if (!EnsureAuthority(GetWorld(), TEXT("InternalDeactivateAI")))
	{
		return;
	}

	if (!AgentInfo.bIsActive)
	{
		return;
	}
	
	AgentInfo.bIsActive = false;
	ApplyStateToAI(AgentInfo, EAIManagerState::Disabled);
	
	APawn* Pawn = AgentInfo.Pawn.Get();
	if (Pawn)
	{
		// Delegate to ActivatableComponent - it handles all the deactivation logic
		if (UActivatableComponent* ActivatableComp = FAIComponentUtils::GetActivatableComponent(Pawn))
		{
			ActivatableComp->DeactivateActor();
		}
		else
		{
			// Fallback if no ActivatableComponent - minimal deactivation
			Pawn->SetActorTickEnabled(false);
			Pawn->SetActorHiddenInGame(true);
			Pawn->SetActorEnableCollision(false);
		}
	}
	
	UE_LOG(CoffeeNet, Log, TEXT("Deactivated AI: %s"), *AgentInfo.DebugName);
}

FAIPoolEntry* UAIManager::FindAvailablePoolEntry(const FString& PoolName)
{
	TArray<FAIPoolEntry>* Pool = ObjectPools.Find(PoolName);
	if (!Pool)
	{
		return nullptr;
	}
	
	for (FAIPoolEntry& Entry : *Pool)
	{
		if (!Entry.bInUse && Entry.Character.IsValid())
		{
			return &Entry;
		}
	}
	
	return nullptr;
}

FAIPoolEntry* UAIManager::FindPoolEntryForPawn(APawn* Pawn)
{
	if (!Pawn)
	{
		return nullptr;
	}
	
	for (auto& PoolPair : ObjectPools)
	{
		for (FAIPoolEntry& Entry : PoolPair.Value)
		{
			if (Entry.Character.Get() == Pawn)
			{
				return &Entry;
			}
		}
	}
	
	return nullptr;
}

void UAIManager::InitializePoolEntry(FAIPoolEntry& PoolEntry, TSubclassOf<ACharacter> CharacterClass,
	TSubclassOf<AAIController> ControllerClass, const FString& PoolName)
{
	if (!EnsureAuthority(GetWorld(), TEXT("InitializePoolEntry")))
	{
		return;
	}

	if (!CharacterClass)
	{
		UE_LOG(CoffeeNet, Error, TEXT("Invalid character class for pool entry"));
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	// Get hidden location from settings
	const UAIManagerSettings* Settings = UAIManagerSettings::Get();
	FVector HiddenLocation = Settings ? Settings->PoolHiddenLocation : FVector(0, 0, -10000);
	
	// Spawn the character at hidden location
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	ACharacter* Character = World->SpawnActor<ACharacter>(CharacterClass, HiddenLocation, FRotator::ZeroRotator, SpawnParams);
	
	if (!Character)
	{
		UE_LOG(CoffeeNet, Error, TEXT("Failed to spawn character for pool"));
		return;
	}
	
	// Spawn and possess with controller if specified
	if (ControllerClass)
	{
		AAIController* Controller = World->SpawnActor<AAIController>(ControllerClass, HiddenLocation, FRotator::ZeroRotator);
		if (Controller)
		{
			Controller->Possess(Character);
			PoolEntry.Controller = Controller;
		}
	}
	
	// Initialize pool entry
	PoolEntry.Character = Character;
	PoolEntry.bInUse = false;
	PoolEntry.PoolName = PoolName;
	PoolEntry.TimesUsed = 0;
	PoolEntry.TimeReturned = World->GetTimeSeconds();
	
	// Deactivate initially using ActivatableComponent if available
	if (UActivatableComponent* ActivatableComp = FAIComponentUtils::GetActivatableComponent(Character))
	{
		ActivatableComp->SetHiddenLocation(HiddenLocation);
		ActivatableComp->DeactivateActor();
	}
	else
	{
		// Fallback: disable ticking if no ActivatableComponent
		Character->SetActorTickEnabled(false);
	}
}

FString UAIManager::BuildInfoString(const FAIAgentInfo& AgentInfo) const
{
	FString StateString = UEnum::GetValueAsString(AgentInfo.CurrentState);
	FString ActiveString = AgentInfo.bIsActive ? TEXT("Active") : TEXT("Inactive");
	
	APawn* Pawn = AgentInfo.Pawn.Get();
	FString LocationString = Pawn ? Pawn->GetActorLocation().ToString() : TEXT("Invalid");
	
	return FString::Printf(TEXT("[%s] %s | State: %s | Location: %s"), 
		*ActiveString, 
		*AgentInfo.DebugName, 
		*StateString, 
		*LocationString);
}

void UAIManager::CleanupInvalidEntries()
{
	if (!EnsureAuthority(GetWorld(), TEXT("CleanupInvalidEntries")))
	{
		return;
	}

	TArray<TWeakObjectPtr<APawn>> InvalidKeys;
	
	for (const auto& Pair : RegisteredAI)
	{
		if (!Pair.Key.IsValid())
		{
			InvalidKeys.Add(Pair.Key);
		}
	}
	
	for (const TWeakObjectPtr<APawn>& Key : InvalidKeys)
	{
		RegisteredAI.Remove(Key);
	}
	
	if (InvalidKeys.Num() > 0)
	{
		UE_LOG(CoffeeNet, Log, TEXT("Cleaned up %d invalid AI entries"), InvalidKeys.Num());
	}
}

const FPoolConfig* UAIManager::FindPoolConfig(const FString& PoolName) const
{
	const UAIManagerSettings* Settings = UAIManagerSettings::Get();
	if (!Settings)
	{
		return nullptr;
	}
	
	return Settings->DefaultPools.FindByPredicate([&PoolName](const FPoolConfig& Cfg) {
		return Cfg.PoolName == PoolName;
	});
}

