#include "Systems/Items/Actors/VinylRecord.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HoldableComponent.h"


AVinylRecord::AVinylRecord()
{
	PrimaryActorTick.bCanEverTick = false;

	//Creation
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
	HoldableComponent = CreateDefaultSubobject<UHoldableComponent>(TEXT("HoldableComponent"));

	//Attachments
	MeshComp->SetupAttachment(GetRootComponent());

	//Default Variables
	MeshComp->SetSimulatePhysics(true);
}

TSubclassOf<UStatMod> AVinylRecord::GetStatMod()
{
	return StatMod;
}

TSubclassOf<UStatImpulse> AVinylRecord::GetStatImpulse()
{
	return StatImpulse;
}

UAkAudioEvent* AVinylRecord::GetPlayEvent()
{
	return PlayEvent;
}

UAkAudioEvent* AVinylRecord::GetStopEvent()
{
	return StopEvent;
}



