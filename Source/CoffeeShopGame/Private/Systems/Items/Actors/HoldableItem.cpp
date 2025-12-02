#include "Systems/Items/Actors/HoldableItem.h"
#include "Net/UnrealNetwork.h"
#include "Systems/Interaction System/Components/HighlightComponent.h"
#include "Systems/PickupDropSystem/HolderComponent/HolderComponent.h"


//Core Shtuff
AHoldableItem::AHoldableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	
	//Replication
	bReplicates = true;
	ABaseItem::SetReplicateMovement(true);
	bNetLoadOnClient = true;
	bNetUseOwnerRelevancy = true;
	bReplicateUsingRegisteredSubObjectList = true;
}

void AHoldableItem::BeginPlay()
{
	Super::BeginPlay();

	UPrimitiveComponent* RootComponentWithPhysics = Cast<UPrimitiveComponent>(RootComponent);
	if (RootComponentWithPhysics) RootComponentWithPhysics->SetSimulatePhysics(bTurnOnPhysics);

	BeginPlaySetupForReplicatedObjects(RootComponentWithPhysics);
}


//Interface
bool AHoldableItem::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	IInteractable::InteractStarted_Implementation(ActionId, Context);

	if (ActionId == EActionId::Q && bCanBeThrown) return ThrowStart(Context);
	
	if (ActionId == EActionId::E)
	{
		if (PickUp(Context)) return true;
		if (Drop(Context)) return true;
	}

	return false;
}

bool AHoldableItem::InteractCompleted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	Super::InteractCompleted_Implementation(ActionId, Context);

	if (ActionId == EActionId::Q && bCanBeThrown) return ThrowEnd(Context);

	return false;
}

bool AHoldableItem::PickUp(const FInteractionContext& Context)
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

bool AHoldableItem::Drop(const FInteractionContext& Context)
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

bool AHoldableItem::ThrowStart(FInteractionContext Context)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return false;
	
	AActor* HeldItemActor = HeldItem->GetActor();
	if (HeldItemActor != this) return false;

	HolderComponent->ServerSide_ThrowItemStart();

	return true;
}

bool AHoldableItem::ThrowEnd(FInteractionContext Context)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return false;
	
	AActor* HeldItemActor = HeldItem->GetActor();
	if (HeldItemActor != this) return false;

	Context.HolderComponent->ServerSide_ThrowItemCompleted(Context.InstigatorController);

	if (HighlightComponent) HighlightComponent->bCanHighlight = true;

	return true;
}


//Replication
void AHoldableItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//adding any properties that should be replicated (not necessary if ThisAsHeldItem is a default suboject --> isn't in this case)
	DOREPLIFETIME(AHoldableItem, ThisAsHeldItem);
}

void AHoldableItem::BeginPlaySetupForReplicatedObjects(UPrimitiveComponent* RootComponentWithPhysics)
{
	//create replicated ThisAsHelditem UObject on Server
	
	if (!HasAuthority()) return;

	if (IsValid(ThisAsHeldItem)) RemoveReplicatedSubObject(ThisAsHeldItem);
	
	ThisAsHeldItem = NewObject<UHeldItem>(this);
	ThisAsHeldItem->Init(this, MeshComp, RootComponentWithPhysics);
	AddReplicatedSubObject(ThisAsHeldItem);
}


