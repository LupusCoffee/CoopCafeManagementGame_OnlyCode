#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "Systems/Items/Actors/VinylRecord.h"
#include "MusicPlayerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UMusicPlayerComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	UMusicPlayerComponent();

	//Methods, Music Update
	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void TryStartMusic();

	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void TryEndMusic();
	
	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void SetVinylRecord(AVinylRecord* InVinylRecord);

	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void UpdateMusic();


	//Methods, Replicate Music Playing
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Play(USoundBase* NetSound);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Stop();


	//Methods, Getters
	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	float GetMinDegreeMusicStartRangeForVinylPlayer();

	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	float GetMaxDegreeMusicStartRangeForVinylPlayer();
	

protected:
	//Variables, Editable --> Static
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	float MinDegreeStartRange = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	float MaxDegreeStartRange = 90.0f;
	
	//Variables, Visible --> Dynamic
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	bool bShouldPlayMusic = false;

	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	bool bCurrentlyPlayingMusic = false;
	
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	AVinylRecord* VinylRecord = nullptr;
};
