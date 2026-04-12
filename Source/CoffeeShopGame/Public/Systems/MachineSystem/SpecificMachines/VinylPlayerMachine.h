#pragma once

#include "CoreMinimal.h"
#include "AkComponent.h"
#include "Systems/MachineSystem/Machine.h"
#include "VinylPlayerMachine.generated.h"
class UStatModApplicationComponent;
class AVinylRecord;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMusicStatusChanged);

UCLASS()
class COFFEESHOPGAME_API AVinylPlayerMachine : public AMachine
{
	GENERATED_BODY()

public:
	AVinylPlayerMachine();

	
	//Methods, Music Update
	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void TryStartMusic();

	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void TryEndMusic();
	
	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void SetVinylRecord(AVinylRecord* InVinylRecord);
	
	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void RemoveVinylRecord();

	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	void UpdateMusic();


	//Machine Status
	virtual void TurnOn() override;
	virtual void TurnOff() override;


	//Methods, Multicast Music Playing
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PostEvent(UAkAudioEvent* Event);


	//Methods, Getters
	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	float GetMinDegreeMusicStartRange();

	UFUNCTION(BlueprintCallable, Category= "Vinyl Player")
	float GetMaxDegreeMusicStartRange();


	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnMusicStatusChanged Server_OnMusicPlay;

	UPROPERTY(BlueprintAssignable)
	FOnMusicStatusChanged Server_OnMusicStop;
	

protected:
	//Methods
	UFUNCTION()
	void PlayMusic(AVinylRecord* Record);
	
	UFUNCTION()
	void StopMusic(AVinylRecord* Record);
	
	
	//Variables, Editable --> Components
	UPROPERTY(EditDefaultsOnly, Category= "Vinyl Player")
	UAkComponent* CustomAudioComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStatModApplicationComponent* StatModApplicationComponent = nullptr;
	
	
	//Variables, Editable --> Static
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Settings | Vinyl Player")
	float MinDegreeStartRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Settings | Vinyl Player")
	float MaxDegreeStartRange = 90.0f;
	
	
	//Variables, Visible --> Dynamic
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	bool bShouldPlayMusic = false;
	
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	bool bHasVinylRecord = false;

	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	bool bCurrentlyPlayingMusic = false;
	
	
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	AVinylRecord* MostRecentVinylRecord = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	UAkAudioEvent* CurrentPlayEvent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category= "Vinyl Player")
	UAkAudioEvent* CurrentStopEvent = nullptr;
};
