#include "Systems/Items/Actors/VinylRecord.h"


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

USoundBase* AVinylRecord::GetSong()
{
	return Song;
}

