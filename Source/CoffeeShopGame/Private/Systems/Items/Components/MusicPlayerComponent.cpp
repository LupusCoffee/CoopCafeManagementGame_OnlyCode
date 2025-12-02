#include "Systems/Items/Components/MusicPlayerComponent.h"
#include "Components/AudioComponent.h"


UMusicPlayerComponent::UMusicPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


//Vinyl Player, Music Update and Playing
void UMusicPlayerComponent::TryStartMusic()
{
	bShouldPlayMusic = true;
	UpdateMusic();
}

void UMusicPlayerComponent::TryEndMusic()
{
	bShouldPlayMusic = false;
	UpdateMusic();
}

void UMusicPlayerComponent::SetVinylRecord(AVinylRecord* InVinylRecord)
{
	VinylRecord = InVinylRecord;
}

void UMusicPlayerComponent::UpdateMusic()
{
	if (VinylRecord && VinylRecord->GetSong() && bShouldPlayMusic && !bCurrentlyPlayingMusic)
	{
		Multicast_Play(VinylRecord->GetSong());
		bCurrentlyPlayingMusic = true;
	}
	else
	{
		Multicast_Stop();
		bCurrentlyPlayingMusic = false;
	}
}


//Play Methods
void UMusicPlayerComponent::Multicast_Play_Implementation(USoundBase* NetSound)
{
	SetSound(NetSound);
	Play();
	
	const bool bServer = GetOwner() && GetOwner()->HasAuthority();
	const FString& Msg = "Play Music";
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, bServer ? FColor::Red : FColor::Green,FString::Printf(TEXT("%s: %s"), bServer ? TEXT("Server") : TEXT("Client"), *Msg));
}


void UMusicPlayerComponent::Multicast_Stop_Implementation()
{
	Stop();

	const bool bServer = GetOwner() && GetOwner()->HasAuthority();
	const FString& Msg = "Stop Music";
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, bServer ? FColor::Red : FColor::Green,FString::Printf(TEXT("%s: %s"), bServer ? TEXT("Server") : TEXT("Client"), *Msg));
}


//Vinyl Player, Getters
float UMusicPlayerComponent::GetMinDegreeMusicStartRangeForVinylPlayer()
{
	return MinDegreeStartRange;
}

float UMusicPlayerComponent::GetMaxDegreeMusicStartRangeForVinylPlayer()
{
	return MaxDegreeStartRange;
}
