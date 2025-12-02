#include "Systems/MachineSystem/MachinePart.h"
#include "Systems/Interaction System/Components/HighlightComponent.h"


//Constructor
AMachinePart::AMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));

	RootComponent = Root;
	MeshComp->SetupAttachment(Root);
}

void AMachinePart::BeginPlay()
{
	Super::BeginPlay();
}

bool AMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!_Owner) return false;
	
	OwnerMachine = _Owner;
	ParentSocket = _ParentSocket;

	return true;
}

void AMachinePart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


//Interface
void AMachinePart::Hover_Implementation(FInteractionContext Context)
{
	IInteractable::Hover_Implementation(Context);
}

void AMachinePart::Unhover_Implementation(FInteractionContext Context)
{
	IInteractable::Unhover_Implementation(Context);
}

bool AMachinePart::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	IInteractable::InteractStarted_Implementation(ActionId, Context);

	return false;
}