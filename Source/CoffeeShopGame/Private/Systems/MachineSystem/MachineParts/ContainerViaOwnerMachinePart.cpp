#include "Systems/MachineSystem/MachineParts/ContainerViaOwnerMachinePart.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionEnum.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"

//Setup
AContainerViaOwnerMachinePart::AContainerViaOwnerMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ContainerComponent = CreateDefaultSubobject<UContainableComponent>("ContainerComponent");
}


//Interaction
void AContainerViaOwnerMachinePart::Local_StartHover_Implementation(FPlayerContext Context)
{
	Super::Local_StartHover_Implementation(Context);
	
	
	if (!ItemPromptComp && !ItemPromptComp->GetPromptBox()) return;
	if (!Context.HolderComponent || !Context.HolderComponent->GetHeldItem()) return;
	
	AActor* HeldItem = Context.HolderComponent->GetHeldItem()->GetActor();
	if (!HeldItem) return;
	
	UContainableComponent* ContainerComp = HeldItem->FindComponentByClass<UContainableComponent>();
	if (!ContainerComp || ContainerComp->GetCurrentTotalVolume() <= 0) return;
	
	ItemPromptComp->GetPromptBox()->SetPrompts({EAction::Pour});
}

void AContainerViaOwnerMachinePart::Local_EndHover_Implementation(FPlayerContext Context)
{
	Super::Local_EndHover_Implementation(Context);
	
	if (!ItemPromptComp && !ItemPromptComp->GetPromptBox()) return;
	ItemPromptComp->GetPromptBox()->ClearPrompts();
}


//Utilities
UContainableComponent* AContainerViaOwnerMachinePart::GetContainerComp()
{
	return ContainerComponent;
}