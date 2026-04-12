#pragma once

#include "CoreMinimal.h"
#include "HoldableItem.h"
#include "Systems/StatModificationSystem/StatImpulse.h"
#include "Systems/StatModificationSystem/StatMod.h"
#include "VinylRecord.generated.h"

class UHoldableComponent;
class UAkAudioEvent;

UCLASS()
class COFFEESHOPGAME_API AVinylRecord : public AActor
{
	GENERATED_BODY()

public:
	AVinylRecord();

	UFUNCTION()
	TSubclassOf<UStatMod> GetStatMod();

	UFUNCTION()
	TSubclassOf<UStatImpulse> GetStatImpulse();
	
	UFUNCTION()
	UAkAudioEvent* GetPlayEvent();

	UFUNCTION()
	UAkAudioEvent* GetStopEvent();

protected:
	//Variables --> Components
	UPROPERTY(EditAnywhere)
	UMeshComponent* MeshComp = nullptr;
	
	UPROPERTY(EditAnywhere)
	UHighlightComponent* HighlightComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UHoldableComponent* HoldableComponent = nullptr;
	
	
	//Variables --> Logic
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	FString SongName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	FString EffectDescription = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	UAkAudioEvent* PlayEvent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	UAkAudioEvent* StopEvent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	TSubclassOf<UStatMod> StatMod = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings")
	TSubclassOf<UStatImpulse> StatImpulse = nullptr;
};
