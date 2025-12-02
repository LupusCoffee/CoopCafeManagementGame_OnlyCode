#include "Systems/Items/Actors/BaseItem.h"

#include "Systems/Interaction System/Components/HighlightComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setup Components
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = MeshComp;

	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));


	//Replication
	MeshComp->SetIsReplicated(true);
}

void ABaseItem::Hover_Implementation(FInteractionContext Context)
{
	IInteractable::Hover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void ABaseItem::Unhover_Implementation(FInteractionContext Context)
{
	IInteractable::Unhover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}

