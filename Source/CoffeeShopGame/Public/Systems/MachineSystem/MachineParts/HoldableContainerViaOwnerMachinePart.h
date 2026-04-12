#pragma once

#include "CoreMinimal.h"
#include "ContainerViaOwnerMachinePart.h"
#include "Systems/MachineSystem/HoldableMachinePart.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HoldableComponent.h"
#include "HoldableContainerViaOwnerMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AHoldableContainerViaOwnerMachinePart : public AHoldableMachinePart
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHoldableContainerViaOwnerMachinePart();

protected:
	//Variables --> Visible, Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UContainableComponent* ContainerComponent = nullptr;
	
	//Container Methods --> Pour Out Of
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext) override;
};
