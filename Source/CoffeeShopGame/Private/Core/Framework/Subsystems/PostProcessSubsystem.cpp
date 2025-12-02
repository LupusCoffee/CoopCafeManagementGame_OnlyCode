#include "Core/Framework/Subsystems/PostProcessSubsystem.h"

//Overrides
void UPostProcessSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (!UnboundPostProcessVolume) UnboundPostProcessVolume = CreateUnboundPostProcessVolume(); //hmmmm --> cause of dependency
	if (!UnboundPostProcessVolume) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red,
		"PostProcessVolume creation in PostProcessSubsystem failed.");
}

void UPostProcessSubsystem::Deinitialize()
{
	UnboundPostProcessVolume = nullptr;
	
	Super::Deinitialize();
}

//Methdos --> Creators
APostProcessVolume* UPostProcessSubsystem::CreateUnboundPostProcessVolume()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	//todo later: check if world already has an unbound post process volume

	//todo: check if world already has an actor with name
	
	FActorSpawnParameters Params;
	Params.Name = MakeUniqueObjectName(World, APostProcessVolume::StaticClass());
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APostProcessVolume* M_PostProcessVolume = World->SpawnActor<APostProcessVolume>(Params);
	if (!M_PostProcessVolume) return nullptr;

	M_PostProcessVolume->bUnbound = true;
	
	return M_PostProcessVolume;
}

//Methdos --> Getters
APostProcessVolume* UPostProcessSubsystem::GetUnboundPostProcessVolume()
{
	if (!UnboundPostProcessVolume) UnboundPostProcessVolume = CreateUnboundPostProcessVolume(); //hmmmm --> cause of dependency
	
	return UnboundPostProcessVolume;
}
