// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoapBlueprintLibrary.h"
#include "../Subsystems/GoapWorldSubsystem.h"
#include "../Components/GoapComponent.h"
#include "GOAPSystem/AI/Utils/AIComponentUtils.h"
#include "Engine/World.h"

// ===== Subsystem Access =====

UGoapWorldSubsystem* UGoapBlueprintLibrary::GetGoapWorldSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return World->GetSubsystem<UGoapWorldSubsystem>();
	}

	return nullptr;
}

// ===== Default World Knowledge =====

void UGoapBlueprintLibrary::SetDefaultActor(const UObject* WorldContextObject, FName Key, AActor* Actor, bool bIsGlobalKnowledge)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->SetDefaultActor(Key, Actor, bIsGlobalKnowledge);
	}
}

void UGoapBlueprintLibrary::SetDefaultLocation(const UObject* WorldContextObject, FName Key, FVector Location, bool bIsGlobalKnowledge)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->SetDefaultLocation(Key, Location, bIsGlobalKnowledge);
	}
}

void UGoapBlueprintLibrary::RemoveDefaultEntry(const UObject* WorldContextObject, FName Key)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->RemoveDefaultEntry(Key);
	}
}

AActor* UGoapBlueprintLibrary::GetDefaultActor(const UObject* WorldContextObject, FName Key)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->GetDefaultActor(Key);
	}

	return nullptr;
}

FVector UGoapBlueprintLibrary::GetDefaultLocation(const UObject* WorldContextObject, FName Key, bool& bIsValid)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->GetDefaultLocation(Key, bIsValid);
	}

	bIsValid = false;
	return FVector::ZeroVector;
}

// ===== Runtime Actor Assignment =====

void UGoapBlueprintLibrary::AssignActorToAgent(AActor* Agent, FName Key, AActor* TargetActor)
{
	if (!Agent)
	{
		return;
	}

	if (UGoapComponent* GoapComp = FAIComponentUtils::GetGoapComponent(Agent))
	{
		GoapComp->AssignTargetActor(Key, TargetActor);
	}
}

void UGoapBlueprintLibrary::AssignActorToAllAgents(const UObject* WorldContextObject, FName Key, AActor* TargetActor)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->AssignActorToAllAgents(Key, TargetActor);
	}
}

void UGoapBlueprintLibrary::RemoveEntryFromAgent(AActor* Agent, FName Key)
{
	if (!Agent)
	{
		return;
	}

	if (UGoapComponent* GoapComp = FAIComponentUtils::GetGoapComponent(Agent))
	{
		GoapComp->RemoveTargetActor(Key);
	}
}

// ===== Shared World State =====

void UGoapBlueprintLibrary::SetSharedBoolState(const UObject* WorldContextObject, FName Key, bool Value)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->SetSharedBoolState(Key, Value);
	}
}

bool UGoapBlueprintLibrary::GetSharedBoolState(const UObject* WorldContextObject, FName Key, bool DefaultValue)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->GetSharedBoolState(Key, DefaultValue);
	}

	return DefaultValue;
}

void UGoapBlueprintLibrary::SetSharedIntState(const UObject* WorldContextObject, FName Key, int32 Value)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->SetSharedIntState(Key, Value);
	}
}

int32 UGoapBlueprintLibrary::GetSharedIntState(const UObject* WorldContextObject, FName Key, int32 DefaultValue)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->GetSharedIntState(Key, DefaultValue);
	}

	return DefaultValue;
}

void UGoapBlueprintLibrary::SetSharedFloatState(const UObject* WorldContextObject, FName Key, float Value)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->SetSharedFloatState(Key, Value);
	}
}

float UGoapBlueprintLibrary::GetSharedFloatState(const UObject* WorldContextObject, FName Key, float DefaultValue)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->GetSharedFloatState(Key, DefaultValue);
	}

	return DefaultValue;
}

// ===== Utility Functions =====

void UGoapBlueprintLibrary::ApplyDefaultKnowledgeToAgent(const UObject* WorldContextObject, AActor* Agent)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->ApplyDefaultKnowledgeToAgent(Agent);
	}
}

void UGoapBlueprintLibrary::ApplySharedStateToAgent(const UObject* WorldContextObject, AActor* Agent)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->ApplySharedStateToAgent(Agent);
	}
}

// ===== Tag-Based Actor Lookup =====

AActor* UGoapBlueprintLibrary::FindActorByTag(const UObject* WorldContextObject, FName Tag)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->FindActorByTag(Tag);
	}
	return nullptr;
}

TArray<AActor*> UGoapBlueprintLibrary::FindActorsByTag(const UObject* WorldContextObject, FName Tag)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->FindActorsByTag(Tag);
	}
	return TArray<AActor*>();
}

AActor* UGoapBlueprintLibrary::GetActorByKeyOrTag(const UObject* WorldContextObject, AActor* Agent, FName Key)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		return Subsystem->GetActorByKeyOrTag(Agent, Key);
	}
	return nullptr;
}

void UGoapBlueprintLibrary::RefreshActorCache(const UObject* WorldContextObject)
{
	if (UGoapWorldSubsystem* Subsystem = GetGoapWorldSubsystem(WorldContextObject))
	{
		Subsystem->RefreshActorCache();
	}
}

