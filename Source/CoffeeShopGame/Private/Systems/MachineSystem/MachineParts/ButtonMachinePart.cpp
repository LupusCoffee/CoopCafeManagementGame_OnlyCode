#include "Systems/MachineSystem/MachineParts/ButtonMachinePart.h"

#include "Systems/Interaction System/Components/HighlightComponent.h"


AButtonMachinePart::AButtonMachinePart()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AButtonMachinePart::Hover_Implementation(FInteractionContext Context)
{
	IInteractable::Hover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void AButtonMachinePart::Unhover_Implementation(FInteractionContext Context)
{
	IInteractable::Unhover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}

bool AButtonMachinePart::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	Super::InteractStarted_Implementation(ActionId, Context);

	if (ActionId != EActionId::E) return false;

	bool MachineIsOn = OwnerMachine->IsOn();
	
	if (MachineIsOn) OwnerMachine->TurnOff();
	else OwnerMachine->TurnOn();

	return true;
}

