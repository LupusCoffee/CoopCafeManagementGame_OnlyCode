#include "Systems/MachineSystem/MachinePart.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"


//Constructor
AMachinePart::AMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
	ItemPromptComp = CreateDefaultSubobject<UItemPromptComponent>(TEXT("PromptComponent"));
	
	RootComponent = Root;
	BaseMeshComp->SetupAttachment(Root);
	ItemPromptComp->SetupAttachment(Root);
}

bool AMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!_Owner) return false;
	
	OwnerMachine = _Owner;
	ParentSocket = _ParentSocket;

	return true;
}