#include "Systems/Items/Actors/VinylRecord.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HoldableComponent.h"


AVinylRecord::AVinylRecord()
{
	PrimaryActorTick.bCanEverTick = false;
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



