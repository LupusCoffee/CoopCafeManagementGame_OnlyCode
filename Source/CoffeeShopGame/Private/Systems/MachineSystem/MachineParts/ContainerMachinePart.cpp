#include "Systems/MachineSystem/MachineParts/ContainerMachinePart.h"

#include "Systems/Interaction System/Components/HighlightComponent.h"

AContainerMachinePart::AContainerMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AContainerMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;

	ContainerComponent = OwnerMachine->GetComponentByClass<UContainerComponent>();
	
	return true;
}

void AContainerMachinePart::Hover_Implementation(FInteractionContext Context)
{
	Super::Hover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void AContainerMachinePart::Unhover_Implementation(FInteractionContext Context)
{
	Super::Unhover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}

void AContainerMachinePart::Recieve_Implementation(EResourceType ResourceTypeToAdd, float FilledVolume)
{
	IFillable::Recieve_Implementation(ResourceTypeToAdd, FilledVolume);

	ContainerComponent->TryAddVolume(ResourceTypeToAdd, FilledVolume);
}
