// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "Systems/MachineSystem/SpecificMachines/VinylPlayerMachine.h"
#include "VinylNeedleMachinePart.generated.h"

class UMusicPlayerComponent;

UCLASS()
class COFFEESHOPGAME_API AVinylNeedleMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	//Constructor & Overrides
	AVinylNeedleMachinePart();
	virtual void BeginPlay() override;
	virtual bool Init(AMachine* _Owner, FName _ParentSocket) override;

protected:
	//Variables --> Visible, Components
	UPROPERTY(BlueprintReadOnly)
	AVinylPlayerMachine* VinylPlayerOwnerMachine = nullptr;


	//Variables --> Hidden, Dynamic Vars
	UPROPERTY()
	bool bShouldTickInteraction = false;

	UPROPERTY()
	float CurrentRotationDegrees;


	//Methods
	UFUNCTION()
	void UpdateRotation(FVector LookedAtLocation);

	UFUNCTION()
	void UpdateMusicBasedOnNeedleRotation();
	
	
	//Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;
	virtual bool InteractOngoing_Implementation(EActionId ActionId, FInteractionContext Context) override;
	virtual bool InteractCompleted_Implementation(EActionId ActionId, FInteractionContext Context) override;
};
