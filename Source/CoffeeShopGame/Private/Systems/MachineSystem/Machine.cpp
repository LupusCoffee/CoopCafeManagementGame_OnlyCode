#include "Systems/MachineSystem/Machine.h"

#include "Net/UnrealNetwork.h"
#include "Systems/Interaction System/Components/HighlightComponent.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "Systems/PickupDropSystem/HolderComponent/HolderComponent.h"


//Constructor, BeginPlay, etc.
AMachine::AMachine()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
	
	RootComponent = MeshComp;

	//Replication
	bReplicateUsingRegisteredSubObjectList = true;
}

void AMachine::BeginPlay()
{
	Super::BeginPlay();

	SetupMachineParts();
}


//Methods
void AMachine::SetupMachineParts()
{
	if (!HasAuthority()) return;
	
	for (auto Element : MachineParts)
	{
		FName SocketName = Element.Key;
		TSubclassOf<AMachinePart> MachinePartClass = Element.Value;
		if (!MachinePartClass) continue;

		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AMachinePart* MachinePart = GetWorld()->SpawnActor<AMachinePart>(MachinePartClass, Params);
		if (!MachinePart) continue;
		
		MachinePart->Init(this, SocketName);
		MachinePart->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void AMachine::TurnOn()
{
	bIsOn = true;
	
	OnMachineActivated.Broadcast();
}

void AMachine::TurnOff()
{
	bIsOn = false;
	
	OnMachineDeactivated.Broadcast();
}

bool AMachine::IsOn()
{
	return bIsOn;
}

UStaticMeshComponent* AMachine::GetMeshComponent()
{
	return MeshComp;
}


//Interfaces
void AMachine::Hover_Implementation(FInteractionContext Context)
{
	IInteractable::Hover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void AMachine::Unhover_Implementation(FInteractionContext Context)
{
	IInteractable::Unhover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}

bool AMachine::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	IInteractable::InteractStarted_Implementation(ActionId, Context);

	if (ActionId == EActionId::Q) PickUpOrDrop(Context);

	return true;
}

void AMachine::PickUpOrDrop(FInteractionContext Context)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();

	if (!HeldItem)
	{
		UHeldItem* NewHeldItem = NewObject<UHeldItem>(HolderComponent);
		NewHeldItem->Init(this, nullptr, nullptr);
		
		HolderComponent->PickUpItem(NewHeldItem);
		
		if (HighlightComponent) HighlightComponent->bCanHighlight = false;
	}
	else if (HeldItem->GetActor() == this)
	{
		HolderComponent->DropItem(Context.LookedAtLocation);	//drop at a "socket" or some kinda grid (idk, we'll see)
		if (HighlightComponent) HighlightComponent->bCanHighlight = true;
	}
}

void AMachine::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachine, bIsOn);
}