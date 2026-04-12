#include "Systems/MachineSystem/MachineParts/ButtonMachinePart.h"

#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionEnum.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"


AButtonMachinePart::AButtonMachinePart()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AButtonMachinePart::Local_StartHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_StartHover_Implementation(Context);
	
	ItemPromptComp->GetPromptBox()->AddPrompts({EAction::MachineInteraction_PressButton});
	ItemPromptComp->SetVisibility(true);

	if (HighlightComponent) HighlightComponent->EnableHighlight();
}

void AButtonMachinePart::Local_EndHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_EndHover_Implementation(Context);
	
	ItemPromptComp->SetVisibility(false);
	ItemPromptComp->GetPromptBox()->ClearPrompts();

	if (HighlightComponent) HighlightComponent->DisableHighlight();
}

bool AButtonMachinePart::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_StartInteraction_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;

	bool MachineIsOn = OwnerMachine->IsOn();
	
	if (MachineIsOn) OwnerMachine->TurnOff();
	else OwnerMachine->TurnOn();

	return true;
}

