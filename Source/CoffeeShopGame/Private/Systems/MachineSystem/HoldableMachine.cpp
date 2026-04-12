#include "Systems/MachineSystem/HoldableMachine.h"
#include "Net/UnrealNetwork.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "Systems/Items/Actors/HoldableItem.h"


AHoldableMachine::AHoldableMachine()
{
	PrimaryActorTick.bCanEverTick = true;

	//Replication
	bReplicates = true;
	AMachine::SetReplicateMovement(true);
	bNetLoadOnClient = true;
	bNetUseOwnerRelevancy = true;
	bReplicateUsingRegisteredSubObjectList = true;
}

void AHoldableMachine::BeginPlay()
{
	Super::BeginPlay();

	UPrimitiveComponent* RootComponentWithPhysics = Cast<UPrimitiveComponent>(RootComponent);
	
	BeginPlaySetupForReplicatedObjects(RootComponentWithPhysics);
}


//Pick Up / Drop Logic
// hover
void AHoldableMachine::Local_StartHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_StartHover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void AHoldableMachine::Local_EndHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_EndHover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}


// interaction start/compelted
bool AHoldableMachine::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_StartInteraction_Implementation(ActionId, Context);

	if (ActionId == EActionId::E)
	{
		if (PickUp(Context)) return true;
		if (Drop(Context)) return true;
	}

	return false;
}


// actual pick/drop methods
bool AHoldableMachine::PickUp(const FPlayerContext& Context)
{
	if (!HasAuthority()) return false;
	if (!ThisAsHeldItem) return false;

	//safety check holder comp by seeing if its owner is the same as the instigator
	
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HolderComponentHeldItem = HolderComponent->GetHeldItem();

	if (!HolderComponentHeldItem) //pick up
	{
		HolderComponent->PickUpItem(ThisAsHeldItem);
		
		if (HighlightComponent) HighlightComponent->bCanHighlight = false;

		return true;
	}


	return false; //did nothing
}

bool AHoldableMachine::Drop(const FPlayerContext& Context)
{
	if (!HasAuthority()) return false;
	if (!ThisAsHeldItem) return false;

	//safety check holder comp by seeing if its owner is the same as the instigator

	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;

	UHeldItem* HolderComponentHeldItem = HolderComponent->GetHeldItem();

	if (HolderComponentHeldItem->GetActor() == this) //drop
	{
		HolderComponent->DropItem(Context.LookedAtLocation);

		if (HighlightComponent) HighlightComponent->bCanHighlight = true;

		return true;
	}

	return false;
}


//Replication
void AHoldableMachine::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AHoldableMachine, ThisAsHeldItem);
}

void AHoldableMachine::BeginPlaySetupForReplicatedObjects(UPrimitiveComponent* RootComponentWithPhysics)
{
	if (!HasAuthority()) return;

	if (IsValid(ThisAsHeldItem)) RemoveReplicatedSubObject(ThisAsHeldItem);
	
	ThisAsHeldItem = NewObject<UHeldItem>(this);
	ThisAsHeldItem->Init(this, MeshComp, RootComponentWithPhysics);
	AddReplicatedSubObject(ThisAsHeldItem);
}
