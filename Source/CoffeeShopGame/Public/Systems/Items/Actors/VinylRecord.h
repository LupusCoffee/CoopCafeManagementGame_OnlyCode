#pragma once

#include "CoreMinimal.h"
#include "HoldableItem.h"
#include "Systems/StatModificationSystem/StatImpulse.h"
#include "Systems/StatModificationSystem/StatMod.h"
#include "VinylRecord.generated.h"

UCLASS()
class COFFEESHOPGAME_API AVinylRecord : public AHoldableItem
{
	GENERATED_BODY()

public:
	AVinylRecord();

	UFUNCTION()
	TSubclassOf<UStatMod> GetStatMod();

	UFUNCTION()
	TSubclassOf<UStatImpulse> GetStatImpulse();
	
	UFUNCTION()
	USoundBase* GetSong();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	FString SongName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	FString EffectDescription = "";

	//wwise event to play
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	USoundBase* Song = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	TSubclassOf<UStatMod> StatMod = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	TSubclassOf<UStatImpulse> StatImpulse = nullptr;
};
