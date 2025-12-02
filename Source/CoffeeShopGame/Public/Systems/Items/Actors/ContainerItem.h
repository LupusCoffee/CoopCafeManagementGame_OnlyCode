#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HoldableItem.h"
#include "NiagaraComponent.h"
#include "Core/Data/Interfaces/Fillable.h"
#include "ContainerItem.generated.h"
class UContainerData;
class UContainerComponent;
class UHeldItem;
class UHighlightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPourStateChange);

UCLASS()
class COFFEESHOPGAME_API AContainerItem : public AHoldableItem, public IFillable
{
	GENERATED_BODY()
	

public:
	AContainerItem();
	virtual void PostInitializeComponents() override;		//refactor this!!!!
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


protected:
	//Variables --> Static, Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UContainerComponent* ContainerComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* PourVfxComp = nullptr;

	//Variables --> Static, Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settings|Data", meta = (ExposeOnSpawn="true", ToolTip = "Setting this will override any values in the Container Component itself, and will instead use this Container Data Asset."))
	UContainerData* ContainerData = nullptr;
	
	//Variables --> Dynamic
	UPROPERTY(VisibleAnywhere)
	bool bIsPouring = false;

	UPROPERTY(VisibleAnywhere)
	FRotator InitialRotation = FRotator::ZeroRotator;


	//Delegates --> Variables, Attach/Detach Item
	UPROPERTY(BlueprintAssignable)
	FOnPourStateChange OnPourStart;

	UPROPERTY(BlueprintAssignable)
	FOnPourStateChange OnPourEnd;


	//Methods --> Interface
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;

	virtual bool InteractOngoing_Implementation(EActionId ActionId, FInteractionContext Context) override;
	
	virtual bool InteractCompleted_Implementation(EActionId ActionId, FInteractionContext InteractorContext);

	virtual void Recieve_Implementation(EResourceType ResourceTypeToAdd, float FilledVolume) override;

	//Methods, Logic
	UFUNCTION()
	bool PourStart(UHolderComponent* HolderCompForPourer, AActor* PourTargetActor,
		UCharacterMovementComponent* MovementComponent, float InPourRate);

	bool UpdatePourRateOngoing(float InPourRate);

	UFUNCTION()
	bool PourEnd(UHolderComponent* HolderCompForPourer, UCharacterMovementComponent* MovementComponent);

	//Methods, Visuals
	UFUNCTION(NetMulticast, Reliable)
	void ActivatePourVFX();
	
	UFUNCTION(NetMulticast, Reliable)
	void DeactivatePourVFX();
};
