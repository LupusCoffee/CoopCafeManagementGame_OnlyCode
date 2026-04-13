#include "Systems/Items/Actors/InteractableActor.h"
#include "Systems/InteractionSystem/Components/HighlightComponent.h"
#include "Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"


//Setup
AInteractableActor::AInteractableActor()
{
	//Tick
	PrimaryActorTick.bCanEverTick = false;
	
	//Setup Components
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
	ItemPromptComp = CreateDefaultSubobject<UItemPromptComponent>(TEXT("PromptComponent"));
	
	//Attach Components
	SetRootComponent(MeshComp);
	ItemPromptComp->SetupAttachment(MeshComp);

	//Replication
	MeshComp->SetIsReplicated(true);
}
