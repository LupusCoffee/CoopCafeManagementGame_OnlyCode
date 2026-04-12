// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoapWorldSubsystem.h"
#include "../Components/GoapComponent.h"
#include "GOAPSystem/AI/Utils/AIComponentUtils.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UGoapWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Initialized"));
}

void UGoapWorldSubsystem::Deinitialize()
{
	DefaultWorldEntries.Empty();
	SharedWorldState.Clear();
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Deinitialized"));
}

void UGoapWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	InitializeDefaultEntries();
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: World begin play, initializing default entries"));
}

// ===== Default World Knowledge Management =====

void UGoapWorldSubsystem::SetDefaultActor(FName Key, AActor* Actor, bool bIsGlobalKnowledge)
{
	if (Key.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot set default actor with empty key"));
		return;
	}

	// Find existing entry or create new one
	FGoapDefaultWorldEntry* ExistingEntry = DefaultWorldEntries.FindByPredicate([Key](const FGoapDefaultWorldEntry& Entry)
	{
		return Entry.Key == Key;
	});

	if (ExistingEntry)
	{
		ExistingEntry->Actor = Actor;
		ExistingEntry->bUseFixedLocation = false;
		ExistingEntry->bIsGlobalKnowledge = bIsGlobalKnowledge;
	}
	else
	{
		FGoapDefaultWorldEntry NewEntry;
		NewEntry.Key = Key;
		NewEntry.Actor = Actor;
		NewEntry.bUseFixedLocation = false;
		NewEntry.bIsGlobalKnowledge = bIsGlobalKnowledge;
		DefaultWorldEntries.Add(NewEntry);
	}

	// If this is global knowledge, update shared world state
	if (bIsGlobalKnowledge && Actor)
	{
		SharedWorldState.SetActorState(Key, Actor);
	}

	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Set default actor for key '%s' (Global: %s)"), 
		*Key.ToString(), bIsGlobalKnowledge ? TEXT("Yes") : TEXT("No"));
}

void UGoapWorldSubsystem::SetDefaultLocation(FName Key, FVector Location, bool bIsGlobalKnowledge)
{
	if (Key.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot set default location with empty key"));
		return;
	}

	// Find existing entry or create new one
	FGoapDefaultWorldEntry* ExistingEntry = DefaultWorldEntries.FindByPredicate([Key](const FGoapDefaultWorldEntry& Entry)
	{
		return Entry.Key == Key;
	});

	if (ExistingEntry)
	{
		ExistingEntry->Location = Location;
		ExistingEntry->bUseFixedLocation = true;
		ExistingEntry->bIsGlobalKnowledge = bIsGlobalKnowledge;
	}
	else
	{
		FGoapDefaultWorldEntry NewEntry;
		NewEntry.Key = Key;
		NewEntry.Location = Location;
		NewEntry.bUseFixedLocation = true;
		NewEntry.bIsGlobalKnowledge = bIsGlobalKnowledge;
		DefaultWorldEntries.Add(NewEntry);
	}

	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Set default location for key '%s' at %s (Global: %s)"), 
		*Key.ToString(), *Location.ToString(), bIsGlobalKnowledge ? TEXT("Yes") : TEXT("No"));
}

void UGoapWorldSubsystem::RemoveDefaultEntry(FName Key)
{
	const int32 RemovedCount = DefaultWorldEntries.RemoveAll([Key](const FGoapDefaultWorldEntry& Entry)
	{
		return Entry.Key == Key;
	});

	if (RemovedCount > 0)
	{
		SharedWorldState.RemoveActorState(Key);
		UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Removed default entry for key '%s'"), *Key.ToString());
	}
}

AActor* UGoapWorldSubsystem::GetDefaultActor(FName Key) const
{
	const FGoapDefaultWorldEntry* Entry = DefaultWorldEntries.FindByPredicate([Key](const FGoapDefaultWorldEntry& Entry)
	{
		return Entry.Key == Key && !Entry.bUseFixedLocation;
	});

	if (Entry && Entry->Actor.IsValid())
	{
		return Entry->Actor.Get();
	}

	return nullptr;
}

FVector UGoapWorldSubsystem::GetDefaultLocation(FName Key, bool& bIsValid) const
{
	const FGoapDefaultWorldEntry* Entry = DefaultWorldEntries.FindByPredicate([Key](const FGoapDefaultWorldEntry& Entry)
	{
		return Entry.Key == Key;
	});

	if (Entry)
	{
		bIsValid = true;
		if (Entry->bUseFixedLocation)
		{
			return Entry->Location;
		}
		else if (Entry->Actor.IsValid())
		{
			return Entry->Actor.Get()->GetActorLocation();
		}
	}

	bIsValid = false;
	return FVector::ZeroVector;
}

bool UGoapWorldSubsystem::HasDefaultEntry(FName Key) const
{
	return DefaultWorldEntries.ContainsByPredicate([Key](const FGoapDefaultWorldEntry& Entry)
	{
		return Entry.Key == Key;
	});
}

// ===== Runtime Actor Assignment =====

void UGoapWorldSubsystem::AssignActorToAgent(AActor* Agent, FName Key, AActor* TargetActor)
{
	if (!Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot assign actor to null agent"));
		return;
	}

	UGoapComponent* GoapComp = GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Agent '%s' does not have a GoapComponent"), *Agent->GetName());
		return;
	}

	GoapComp->SetObjectState(Key, TargetActor);
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Assigned actor for key '%s' to agent '%s'"), 
		*Key.ToString(), *Agent->GetName());
}

void UGoapWorldSubsystem::AssignLocationToAgent(AActor* Agent, FName Key, FVector Location)
{
	if (!Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot assign location to null agent"));
		return;
	}

	UGoapComponent* GoapComp = GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Agent '%s' does not have a GoapComponent"), *Agent->GetName());
		return;
	}

	// Store location as a vector state (could extend world state to support this better)
	// For now, we'll log a warning that location assignment needs an actor placeholder
	UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Direct location assignment requires an actor placeholder. Consider using an empty actor at the location."));
}

void UGoapWorldSubsystem::RemoveEntryFromAgent(AActor* Agent, FName Key)
{
	if (!Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot remove entry from null agent"));
		return;
	}

	UGoapComponent* GoapComp = GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Agent '%s' does not have a GoapComponent"), *Agent->GetName());
		return;
	}

	GoapComp->RemoveTargetActor(Key);
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Removed entry for key '%s' from agent '%s'"), 
		*Key.ToString(), *Agent->GetName());
}

void UGoapWorldSubsystem::AssignActorToAllAgents(FName Key, AActor* TargetActor)
{
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot assign null actor to agents"));
		return;
	}

	// Update shared state
	SharedWorldState.SetActorState(Key, TargetActor);

	// Find all GOAP components in the world
	TArray<UGoapComponent*> GoapComponents;
	for (TObjectIterator<UGoapComponent> It; It; ++It)
	{
		if (It->GetWorld() == GetWorld())
		{
			GoapComponents.Add(*It);
		}
	}

	// Assign to all agents
	for (UGoapComponent* GoapComp : GoapComponents)
	{
		if (GoapComp && GoapComp->GetOwner())
		{
			GoapComp->SetObjectState(Key, TargetActor);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Assigned actor for key '%s' to %d agents"), 
		*Key.ToString(), GoapComponents.Num());
}

void UGoapWorldSubsystem::AssignLocationToAllAgents(FName Key, FVector Location)
{
	UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Direct location assignment to all agents requires an actor placeholder. Consider using SetDefaultLocation and ApplyDefaultKnowledgeToAgent."));
}

// ===== Shared World State =====

void UGoapWorldSubsystem::SetSharedBoolState(FName Key, bool Value)
{
	SharedWorldState.SetBoolState(Key, Value);
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Set shared bool state '%s' = %s"), 
		*Key.ToString(), Value ? TEXT("true") : TEXT("false"));
}

bool UGoapWorldSubsystem::GetSharedBoolState(FName Key, bool DefaultValue) const
{
	return SharedWorldState.GetBoolState(Key, DefaultValue);
}

void UGoapWorldSubsystem::SetSharedIntState(FName Key, int32 Value)
{
	SharedWorldState.SetIntState(Key, Value);
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Set shared int state '%s' = %d"), 
		*Key.ToString(), Value);
}

int32 UGoapWorldSubsystem::GetSharedIntState(FName Key, int32 DefaultValue) const
{
	return SharedWorldState.GetIntState(Key, DefaultValue);
}

void UGoapWorldSubsystem::SetSharedFloatState(FName Key, float Value)
{
	SharedWorldState.SetFloatState(Key, Value);
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Set shared float state '%s' = %f"), 
		*Key.ToString(), Value);
}

float UGoapWorldSubsystem::GetSharedFloatState(FName Key, float DefaultValue) const
{
	return SharedWorldState.GetFloatState(Key, DefaultValue);
}

void UGoapWorldSubsystem::ApplySharedStateToAgent(AActor* Agent)
{
	if (!Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot apply shared state to null agent"));
		return;
	}

	UGoapComponent* GoapComp = GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Agent '%s' does not have a GoapComponent"), *Agent->GetName());
		return;
	}

	GoapComp->ApplyEffects(SharedWorldState);
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Applied shared state to agent '%s'"), *Agent->GetName());
}

void UGoapWorldSubsystem::ApplyDefaultKnowledgeToAgent(AActor* Agent)
{
	if (!Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot apply default knowledge to null agent"));
		return;
	}

	UGoapComponent* GoapComp = GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Agent '%s' does not have a GoapComponent"), *Agent->GetName());
		return;
	}

	// Apply all default entries marked as global knowledge
	for (const FGoapDefaultWorldEntry& Entry : DefaultWorldEntries)
	{
		if (Entry.bIsGlobalKnowledge)
		{
			if (!Entry.bUseFixedLocation && Entry.Actor.IsValid())
			{
				GoapComp->SetObjectState(Entry.Key, Entry.Actor.Get());
			}
		}
	}

	// Also apply shared world state
	GoapComp->ApplyEffects(SharedWorldState);

	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Applied default knowledge to agent '%s'"), *Agent->GetName());
}

// ===== Query Functions =====

TArray<AActor*> UGoapWorldSubsystem::FindAllActorsForKey(FName Key) const
{
	TArray<AActor*> Result;

	// Find all GOAP components in the world
	for (TObjectIterator<UGoapComponent> It; It; ++It)
	{
		if (It->GetWorld() == GetWorld())
		{
			if (AActor* Actor = It->GetWorldState().GetActorState(Key))
			{
				Result.AddUnique(Actor);
			}
		}
	}

	return Result;
}

int32 UGoapWorldSubsystem::CountAgentsWithActor(FName Key, AActor* Actor) const
{
	if (!Actor)
	{
		return 0;
	}

	int32 Count = 0;

	// Find all GOAP components in the world
	for (TObjectIterator<UGoapComponent> It; It; ++It)
	{
		if (It->GetWorld() == GetWorld())
		{
			if (It->GetWorldState().GetActorState(Key) == Actor)
			{
				Count++;
			}
		}
	}

	return Count;
}


// ===== Tag-Based Actor Lookup =====

AActor* UGoapWorldSubsystem::FindActorByTag(FName Tag) const
{
	// Check cache first
	if (const TObjectPtr<AActor>* CachedActor = CachedActorsByTag.Find(Tag))
	{
		if (*CachedActor && (*CachedActor)->IsValidLowLevel())
		{
			return CachedActor->Get();
		}
	}

	// Cache miss or invalid - search for actor
	if (UWorld* World = GetWorld())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(World, Tag, FoundActors);
		
		if (FoundActors.Num() > 0)
		{
			AActor* FoundActor = FoundActors[0];
			// Update cache
			const_cast<UGoapWorldSubsystem*>(this)->CachedActorsByTag.Add(Tag, FoundActor);
			
			UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Found actor '%s' with tag '%s'"), 
				*FoundActor->GetName(), *Tag.ToString());
			
			return FoundActor;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: No actor found with tag '%s'"), *Tag.ToString());
	return nullptr;
}

TArray<AActor*> UGoapWorldSubsystem::FindActorsByTag(FName Tag) const
{
	TArray<AActor*> FoundActors;
	
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsWithTag(World, Tag, FoundActors);
		
		UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Found %d actors with tag '%s'"), 
			FoundActors.Num(), *Tag.ToString());
	}
	
	return FoundActors;
}

AActor* UGoapWorldSubsystem::GetActorByKeyOrTag(AActor* Agent, FName Key) const
{
	if (!Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapWorldSubsystem: Cannot get actor for null agent"));
		return nullptr;
	}

	// First, try to get from agent's world state (manually assigned actors)
	UGoapComponent* GoapComp = GetGoapComponent(Agent);
	if (GoapComp)
	{
		AActor* AssignedActor = GoapComp->GetWorldState().GetActorState(Key);
		if (AssignedActor)
		{
			return AssignedActor;
		}
	}

	// Second, check default world knowledge
	AActor* DefaultActor = GetDefaultActor(Key);
	if (DefaultActor)
	{
		return DefaultActor;
	}

	// Finally, fall back to tag-based lookup
	return FindActorByTag(Key);
}

void UGoapWorldSubsystem::RefreshActorCache()
{
	CachedActorsByTag.Empty();
	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Actor cache cleared"));
}

// ===== Private Helper Functions =====


UGoapComponent* UGoapWorldSubsystem::GetGoapComponent(AActor* Agent) const
{
	return FAIComponentUtils::GetGoapComponent(Agent);
}

void UGoapWorldSubsystem::InitializeDefaultEntries()
{
	// Resolve any soft object pointers in default entries
	for (FGoapDefaultWorldEntry& Entry : DefaultWorldEntries)
	{
		if (!Entry.bUseFixedLocation && Entry.Actor.IsValid())
		{
			if (AActor* LoadedActor = Entry.Actor.Get())
			{
				if (Entry.bIsGlobalKnowledge)
				{
					SharedWorldState.SetActorState(Entry.Key, LoadedActor);
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GoapWorldSubsystem: Initialized %d default world entries"), DefaultWorldEntries.Num());
}
