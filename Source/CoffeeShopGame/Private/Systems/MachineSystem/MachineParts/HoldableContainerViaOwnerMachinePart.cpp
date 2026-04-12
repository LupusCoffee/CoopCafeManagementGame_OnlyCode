#include "Systems/MachineSystem/MachineParts/HoldableContainerViaOwnerMachinePart.h"

AHoldableContainerViaOwnerMachinePart::AHoldableContainerViaOwnerMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;

	ContainerComponent = CreateDefaultSubobject<UContainableComponent>("ContainerComponent");
}

bool AHoldableContainerViaOwnerMachinePart::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext)
{
	return Super::Server_StartInteraction_Implementation(ActionId, PlayerContext);
}