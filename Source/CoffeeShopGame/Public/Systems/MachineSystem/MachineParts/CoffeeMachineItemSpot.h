#pragma once

#include "CoreMinimal.h"
#include "ItemSpotMachinePart.h"
#include "Systems/Items/Components/ContainerComponent.h"
#include "Core/Components/PouringComponent.h"
#include "CoffeeMachineItemSpot.generated.h"

UCLASS()
class COFFEESHOPGAME_API ACoffeeMachineItemSpot : public AItemSpotMachinePart
{
	GENERATED_BODY()

public:
	//Constructor
	ACoffeeMachineItemSpot();

protected:
	//Core Overrides
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//Other Overrides
	virtual bool Init(AMachine* _Owner, FName _ParentSocket) override;
	virtual void MachineActiveUpdate(float DeltaTime) override;

	
	//Variables
	UPROPERTY()
	UPouringComponent* PouringComponentOfOwnerMachine = nullptr;

	UPROPERTY()
	UContainerComponent* ContainerComponentOfOwnerMachine = nullptr;
	
	
	//Methods --> Subscribed to Events
	UFUNCTION()
	void OnMachineActivated();

	UFUNCTION()
	void OnMachineDeactivated();
	

	//Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;

	//Methods --> interface helpers
	UFUNCTION()
	void SetHighlightMesh(UHeldItem* ItemToHighlight);

};
