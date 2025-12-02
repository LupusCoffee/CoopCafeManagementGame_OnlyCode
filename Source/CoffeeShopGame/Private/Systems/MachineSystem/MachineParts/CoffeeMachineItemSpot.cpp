#include "Systems/MachineSystem/MachineParts/CoffeeMachineItemSpot.h"

//Setup
ACoffeeMachineItemSpot::ACoffeeMachineItemSpot()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACoffeeMachineItemSpot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Unsubscribe to delegates in owner?
	/*OwnerMachine->OnMachineActivated.RemoveDynamic(this, &ACoffeeMachineItemSpot::OnMachineActivated);
	OwnerMachine->OnMachineDeactivated.RemoveDynamic(this, &ACoffeeMachineItemSpot::OnMachineDeactivated);*/
	
	Super::EndPlay(EndPlayReason);
}

bool ACoffeeMachineItemSpot::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;
	
	//Set Components
	PouringComponentOfOwnerMachine = OwnerMachine->GetComponentByClass<UPouringComponent>();
	ContainerComponentOfOwnerMachine = OwnerMachine->GetComponentByClass<UContainerComponent>();

	//Subscribe to delegates in owner
	OwnerMachine->OnMachineActivated.AddDynamic(this, &ACoffeeMachineItemSpot::OnMachineActivated);
	OwnerMachine->OnMachineDeactivated.AddDynamic(this, &ACoffeeMachineItemSpot::OnMachineDeactivated);

	//turns mesh comp invisible, but still highlightable --> for switching its mesh based on what we are holding (this stupid?)
	MeshComp->SetRenderInMainPass(false);
	MeshComp->SetRenderInDepthPass(false);
	MeshComp->SetCastShadow(false);
	
	return true;
}


//Methods
void ACoffeeMachineItemSpot::MachineActiveUpdate(float DeltaTime)
{
	Super::MachineActiveUpdate(DeltaTime);

	AActor* ItemAtSpotActor = nullptr;
	if (ItemAtSpot) ItemAtSpotActor = ItemAtSpot->GetActor();
	if (!ItemAtSpotActor) return;
	
	ContainerComponentOfOwnerMachine->PourIntoTargetTick(DeltaTime, ItemAtSpotActor);
}


//Methods --> Subscribed to Events
void ACoffeeMachineItemSpot::OnMachineActivated()
{
	if (PouringComponentOfOwnerMachine) PouringComponentOfOwnerMachine->StartVFX();
}

void ACoffeeMachineItemSpot::OnMachineDeactivated()
{
	if (PouringComponentOfOwnerMachine) PouringComponentOfOwnerMachine->StopVFX();
}


//Interface, Hover/Unhover
void ACoffeeMachineItemSpot::Hover_Implementation(FInteractionContext Context)
{
	Super::Hover_Implementation(Context);

	
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent)
	{
		if (ItemAtSpot) return;
		SetHighlightMesh(ItemAtSpot);
		return;
	}

	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem)
	{
		if (!ItemAtSpot) return;
		SetHighlightMesh(ItemAtSpot);
		return;
	}

	
	SetHighlightMesh(HeldItem);
}

void ACoffeeMachineItemSpot::Unhover_Implementation(FInteractionContext Context)
{
	Super::Unhover_Implementation(Context);

	MeshComp->SetStaticMesh(nullptr);
}

void ACoffeeMachineItemSpot::SetHighlightMesh(UHeldItem* ItemToHighlight)
{
	if (!ItemToHighlight) return;
	
	UStaticMeshComponent* StaticMeshComponent = ItemToHighlight->GetMeshComp();
	if (!StaticMeshComponent) return;
	
	UStaticMesh* HighlightMesh = StaticMeshComponent->GetStaticMesh();
	if (!HighlightMesh) return;
	
	MeshComp->SetStaticMesh(HighlightMesh);
	MeshComp->SetWorldScale3D(StaticMeshComponent->GetComponentScale());
}