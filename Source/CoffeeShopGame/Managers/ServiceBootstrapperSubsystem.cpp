#include "ServiceBootstrapperSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "ServiceBootstrapperSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogServiceBootstrapper, Log, All);

namespace
{
	bool IsAuthorityWorld(const UWorld* World)
	{
		return World && World->GetNetMode() != NM_Client;
	}
}

void UServiceBootstrapperSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	BootstrapServices();
}

void UServiceBootstrapperSubsystem::BootstrapServices()
{
	if (bHasBootstrapped)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const UServiceBootstrapperSettings* Settings = UServiceBootstrapperSettings::Get();
	if (!Settings)
	{
		UE_LOG(LogServiceBootstrapper, Warning, TEXT("ServiceBootstrapper: settings missing."));
		return;
	}

	if (Settings->bSpawnOnlyOnAuthority && !IsAuthorityWorld(World))
	{
		UE_LOG(LogServiceBootstrapper, Verbose, TEXT("ServiceBootstrapper: skipping spawn on client world."));
		bHasBootstrapped = true;
		return;
	}

	FActorSpawnParameters SpawnParams;
	const ESpawnActorCollisionHandlingMethod CollisionHandling =
		Settings->SpawnCollisionHandling == ESpawnActorCollisionHandlingMethod::Undefined
			? ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			: Settings->SpawnCollisionHandling;
	SpawnParams.SpawnCollisionHandlingOverride = CollisionHandling;

	TSet<UClass*> UniqueClasses;
	for (const TSoftClassPtr<AActor>& SoftClass : Settings->ActorClassesToSpawn)
	{
		UClass* ClassToSpawn = SoftClass.LoadSynchronous();
		if (!ClassToSpawn)
		{
			UE_LOG(LogServiceBootstrapper, Warning, TEXT("ServiceBootstrapper: invalid class '%s'"), *SoftClass.ToString());
			continue;
		}

		if (UniqueClasses.Contains(ClassToSpawn))
		{
			continue;
		}
		UniqueClasses.Add(ClassToSpawn);

		if (AActor* SpawnedActor = World->SpawnActor<AActor>(ClassToSpawn, FTransform::Identity, SpawnParams))
		{
			SpawnedServiceActors.Add(SpawnedActor);
			UE_LOG(LogServiceBootstrapper, Log, TEXT("ServiceBootstrapper: spawned %s"), *SpawnedActor->GetName());
		}
		else
		{
			UE_LOG(LogServiceBootstrapper, Warning, TEXT("ServiceBootstrapper: failed to spawn class %s"), *ClassToSpawn->GetName());
		}
	}

	bHasBootstrapped = true;
}

void UServiceBootstrapperSubsystem::Deinitialize()
{
	SpawnedServiceActors.Empty();
	bHasBootstrapped = false;
	Super::Deinitialize();
}


