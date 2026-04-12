#include "Systems/MachineSystem/MachineParts/WindingHandleStaticOnOffMachinePart.h"

AWindingHandleStaticOnOffMachinePart::AWindingHandleStaticOnOffMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AWindingHandleStaticOnOffMachinePart::Server_StartInteraction_Implementation(EActionId ActionId,
	FPlayerContext Context)
{
	OwnerMachine->TurnOn();
	
	return Super::Server_StartInteraction_Implementation(ActionId, Context);
}

bool AWindingHandleStaticOnOffMachinePart::Server_EndInteraction_Implementation(EActionId ActionId,
	FPlayerContext Context)
{
	OwnerMachine->TurnOff();
	
	return Super::Server_EndInteraction_Implementation(ActionId, Context);
}

