#include "AudioPlayerComponent.h"


UAudioPlayerComponent::UAudioPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAudioPlayerComponent::SetInitialAndMainSound(USoundBase* InInitial, USoundBase* InMain)
{
	InitialSound = InInitial;
	MainSound = InMain;
}

void UAudioPlayerComponent::PlayInitialAndMain()
{
	if (bIncludeInitialSound && InitialSound) PlaySound(InitialSound, false);
	else PlaySound(MainSound, bLoopMainSound);
}

void UAudioPlayerComponent::StopInitialAndMain()
{
	OnAudioFinished.RemoveDynamic(this, &UAudioPlayerComponent::HandleLoop);
	OnAudioFinished.RemoveDynamic(this, &UAudioPlayerComponent::HandleInitialToMainTransition);
	StopSound();
}

void UAudioPlayerComponent::PlaySound(USoundBase* InSound, bool bLoop)
{
	if (!InSound) return;
	
	SetSound(InSound);
	CurrentSound = InSound;
	Play();

	if (CurrentSound == InitialSound) OnAudioFinished.AddDynamic(this, &UAudioPlayerComponent::HandleInitialToMainTransition);

	if (bLoop)
	{
		bCurrentSoundLooping = true;
		OnAudioFinished.AddDynamic(this, &UAudioPlayerComponent::HandleLoop);
	}
}

void UAudioPlayerComponent::StopSound()
{
	Stop();
	
	CurrentSound = nullptr;
	bCurrentSoundLooping = false;
	OnAudioFinished.RemoveDynamic(this, &UAudioPlayerComponent::HandleLoop);
}


void UAudioPlayerComponent::HandleLoop()
{
	Play();
}

void UAudioPlayerComponent::HandleInitialToMainTransition()
{
	if (MainSound) PlaySound(MainSound, bLoopMainSound);
	OnAudioFinished.RemoveDynamic(this, &UAudioPlayerComponent::HandleInitialToMainTransition);
}
