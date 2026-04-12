#include "Systems/MachineSystem/MachineParts/BaseWindingHandleMachinePart.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionEnum.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"


//Setup and Tick
ABaseWindingHandleMachinePart::ABaseWindingHandleMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;
}


//hover & interact
void ABaseWindingHandleMachinePart::Local_StartHover_Implementation(FPlayerContext Context)
{
	Super::Local_StartHover_Implementation(Context);
	
	ItemPromptComp->GetPromptBox()->AddPrompts({EAction::MachineInteraction_Turn});
	ItemPromptComp->SetVisibility(true);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void ABaseWindingHandleMachinePart::Local_EndHover_Implementation(FPlayerContext Context)
{
	Super::Local_EndHover_Implementation(Context);
	
	if (!ItemPromptComp) return;
	
	ItemPromptComp->SetVisibility(false);
	ItemPromptComp->GetPromptBox()->ClearPrompts();

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}

bool ABaseWindingHandleMachinePart::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_StartInteraction_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;

	if (!Context.MovementComponent) return false;
	Context.MovementComponent->SetMovementMode(MOVE_None);

	bShouldTickInteraction = true;
	
	return true;
}

bool ABaseWindingHandleMachinePart::Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_EndInteraction_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;

	if (!Context.MovementComponent) return false;
	Context.MovementComponent->SetMovementMode(MOVE_Walking);

	bShouldTickInteraction = false;
	
	return true;
}


//tick
void ABaseWindingHandleMachinePart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bShouldTickInteraction) RotateHandleTick(DeltaTime);
}

float ABaseWindingHandleMachinePart::RotateHandleTick(float DeltaTime)
{
	float RotationAmount = DeltaTime;
	if (BaseMeshComp) BaseMeshComp->AddLocalRotation(FRotator(0, RotationAmount * VisualHandleSpinRate, 0));

	return RotationAmount;
}

