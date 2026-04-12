#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "AudioPlayerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UAudioPlayerComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	UAudioPlayerComponent();


	UFUNCTION(BlueprintCallable)
	void SetInitialAndMainSound(USoundBase* InInitial, USoundBase* InMain);
	
	UFUNCTION(BlueprintCallable)
	void PlayInitialAndMain();

	UFUNCTION(BlueprintCallable)
	void StopInitialAndMain();
	
	UFUNCTION(BlueprintCallable)
	void PlaySound(USoundBase* InSound, bool bLoop);

	UFUNCTION(BlueprintCallable)
	void StopSound();

	
	UPROPERTY(EditAnywhere)
	bool bIncludeInitialSound = true;
	
	UPROPERTY(EditAnywhere)
	bool bLoopMainSound = true;


	
protected:
	UFUNCTION()
	void HandleLoop();
	
	UFUNCTION()
	void HandleInitialToMainTransition();
	

	UPROPERTY()
	USoundBase* InitialSound = nullptr;

	UPROPERTY()
	USoundBase* MainSound = nullptr;
	
	UPROPERTY()
	USoundBase* CurrentSound = nullptr;
	
	UPROPERTY()
	bool bCurrentSoundLooping = false;
};
