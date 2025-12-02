#include "Systems/MachineSystem/SpecificMachines/VinylPlayerMachine.h"
#include "Systems/Items/Actors/VinylRecord.h"
#include "Components/AudioComponent.h"
#include "Systems/StatModificationSystem/Components/StatModApplicationComponent.h"


AVinylPlayerMachine::AVinylPlayerMachine()
{
	PrimaryActorTick.bCanEverTick = false;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

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

void AVinylPlayerMachine::SetVinylRecord(AVinylRecord* InVinylRecord)
{
	CurrentVinylRecord = InVinylRecord;

	if (StatModApplicationComponent)
	{
		if (CurrentVinylRecord)
		{
			if (CurrentVinylRecord->GetStatImpulse())
			{
				UStatImpulse* StatImpulse = NewObject<UStatImpulse>(this, CurrentVinylRecord->GetStatImpulse());
				StatModApplicationComponent->SetStatImpulse(StatImpulse);
			}

			if (CurrentVinylRecord->GetStatMod())
			{
				UStatMod* StatMod = NewObject<UStatMod>(this, CurrentVinylRecord->GetStatMod());
				StatModApplicationComponent->SetStatMod(StatMod);
			}
		}
		else StatModApplicationComponent->SetStatMod(nullptr);
	}
	
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

void AVinylPlayerMachine::UpdateMusic()
{
	if (bIsOn && bShouldPlayMusic && CurrentVinylRecord)
	{
		if (bCurrentlyPlayingMusic) return;
		
		Server_OnMusicPlay.Broadcast();
		Multicast_Play(CurrentVinylRecord->GetSong());

		if (StatModApplicationComponent) StatModApplicationComponent->UpdateStatHandlers(true);
		
		bCurrentlyPlayingMusic = true;
	}
	else
	{
		Server_OnMusicStop.Broadcast();
		Multicast_Stop();

		if (StatModApplicationComponent) StatModApplicationComponent->UpdateStatHandlers(false);
		
		bCurrentlyPlayingMusic = false;
	}
}


//Play Methods
void AVinylPlayerMachine::Multicast_Play_Implementation(USoundBase* NetSound)
{
	AudioComponent->SetSound(NetSound);
	AudioComponent->Play();
}


void AVinylPlayerMachine::Multicast_Stop_Implementation()
{
	AudioComponent->Stop();
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
