//Note: move to "core/components" if used by more than interaction system


#include "Systems/Interaction System/Components/HighlightComponent.h"
#include "Systems/Interaction System/Subsystems/HighlightRegistrySubsystem.h"


UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	HighlightedMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	HighlightRegistry = GetWorld()->GetSubsystem<UHighlightRegistrySubsystem>();
	
	if (!HighlightedMesh)
	{
		FString OwnerName = GetOwner()->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red,
		   FString::Printf(TEXT("Highlighted mesh on actor '%s' with HighlightComponent is null"), *OwnerName));
	}
	else HighlightedMesh->SetRenderCustomDepth(false);
	
	if (!HighlightRegistry) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red,
		"Highlight registry is null.");
}


void UHighlightComponent::EnableHighlight()
{
	//todo: StencilIDs should be sent as an argument in "EnableHighlight" if we are ever to highlight multiple objects

	if (!bCanHighlight) return;
	
	if (HighlightedMesh && HighlightRegistry)
	{
		HighlightRegistry->UpdateHighlightMaterial(ActorType);			//for the future, each highlight component CANNOT update the highlight mat in PostProcessSubsystem --> yuckie
		HighlightedMesh->SetRenderCustomDepth(true);
	}
}

void UHighlightComponent::DisableHighlight()
{
	HighlightedMesh->SetRenderCustomDepth(false);
}

