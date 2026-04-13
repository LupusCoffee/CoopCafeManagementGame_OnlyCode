#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Subsystems/HighlightRegistrySubsystem.h"


//Setup
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


//Enabled / Disable
void UHighlightComponent::EnableHighlight()
{
	if (!bCanHighlight) return;
	
	if (HighlightedMesh && HighlightRegistry)
	{
		HighlightRegistry->UpdateHighlightMaterial(ActorType);
		HighlightedMesh->SetRenderCustomDepth(true);
	}
}

void UHighlightComponent::DisableHighlight()
{
	HighlightedMesh->SetRenderCustomDepth(false);
}


//Auto Highlight
void UHighlightComponent::Local_StartHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_StartHover_Implementation(Context);
	
	EnableHighlight();
}

void UHighlightComponent::Local_EndHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_EndHover_Implementation(Context);
	
	DisableHighlight();
}
