#include "Systems/MachineSystem/SpecificMachines/VinylPlayerMachine.h"
#include "Systems/Items/Actors/VinylRecord.h"
#include "Systems/StatModificationSystem/Components/StatModApplicationComponent.h"


AVinylPlayerMachine::AVinylPlayerMachine()
{
	PrimaryActorTick.bCanEverTick = false;

	CustomAudioComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AudioComponent"));
	CustomAudioComponent->SetupAttachment(RootComponent);

	StatModApplicationComponent = CreateDefaultSubobject<UStatModApplicationComponent>(TEXT("StatModApplicationComponent"));
}


//Music Update and Playing
void AVinylPlayerMachine::TryStartMusic()
{
	bShouldPlayMusic = true;
	UpdateMusic();
}

void AVinylPlayerMachine::TryEndMusic()
{
	bShouldPlayMusic = false;
	UpdateMusic();
}

void AVinylPlayerMachine::TurnOn()
{
	Super::TurnOn();
	UpdateMusic();
}

void AVinylPlayerMachine::TurnOff()
{
	Super::TurnOff();
	UpdateMusic();
}

void AVinylPlayerMachine::SetVinylRecord(AVinylRecord* InVinylRecord)
{
	//null checks
	if (!StatModApplicationComponent) return;
	if (!InVinylRecord) return;
	
	
	//set stat mod
	if (InVinylRecord->GetStatImpulse())
	{
		UStatImpulse* StatImpulse = NewObject<UStatImpulse>(this, InVinylRecord->GetStatImpulse());
		StatModApplicationComponent->SetStatImpulse(StatImpulse);
	}

	if (InVinylRecord->GetStatMod())
	{
		UStatMod* StatMod = NewObject<UStatMod>(this, InVinylRecord->GetStatMod());
		StatModApplicationComponent->SetStatMod(StatMod);
	}
	
	
	//set vinyl
	MostRecentVinylRecord = InVinylRecord;
	bHasVinylRecord = true;
	
	
	//update
	UpdateMusic();
}

void AVinylPlayerMachine::RemoveVinylRecord()
{	
	bHasVinylRecord = false;
	
	if (StatModApplicationComponent) StatModApplicationComponent->SetStatMod(nullptr);
	
	UpdateMusic();
}

void AVinylPlayerMachine::UpdateMusic()
{
	if (bIsOn && bShouldPlayMusic && bHasVinylRecord)
	{		
		if (bCurrentlyPlayingMusic) return;
		
		Server_OnMusicPlay.Broadcast();
		PlayMusic(MostRecentVinylRecord);
	}
	else
	{
		Server_OnMusicStop.Broadcast();
		StopMusic(MostRecentVinylRecord);
	}
}

void AVinylPlayerMachine::PlayMusic(AVinylRecord* Record)
{
	if (Record) Multicast_PostEvent(Record->GetPlayEvent());

	if (StatModApplicationComponent) StatModApplicationComponent->UpdateStatHandlers(true);
		
	bCurrentlyPlayingMusic = true;
}

void AVinylPlayerMachine::StopMusic(AVinylRecord* Record)
{
	if (Record) Multicast_PostEvent(Record->GetStopEvent());

	if (StatModApplicationComponent) StatModApplicationComponent->UpdateStatHandlers(false);
		
	bCurrentlyPlayingMusic = false;
}


//Play Methods
void AVinylPlayerMachine::Multicast_PostEvent_Implementation(UAkAudioEvent* Event)
{
	if (!Event) return;
	CustomAudioComponent->PostAkEvent(Event);
}


//Getters
float AVinylPlayerMachine::GetMinDegreeMusicStartRange()
{
	return MinDegreeStartRange;
}

float AVinylPlayerMachine::GetMaxDegreeMusicStartRange()
{
	return MaxDegreeStartRange;
}