#include "Systems/MachineSystem/HoldableMachinePart.h"
#include "Net/UnrealNetwork.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionEnum.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"


AHoldableMachinePart::AHoldableMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHoldableMachinePart::BeginPlay()
{
	Super::BeginPlay();

	UPrimitiveComponent* RootComponentWithPhysics = Cast<UPrimitiveComponent>(RootComponent);
	
	BeginPlaySetupForReplicatedObjects(RootComponentWithPhysics);
}


// interaction start/compelted
bool AHoldableMachinePart::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_StartInteraction_Implementation(ActionId, Context);

	if (ActionId == EActionId::E)
	{
		if (PickUp(Context)) return true;
		if (Context.LookedAtActor == OwnerMachine && AttachToParentMachine(Context)) return true;
		if (Drop(Context)) return true;
	}

	return false;
}


// actual pick/drop methods
bool AHoldableMachinePart::AttachToParentMachine(const FPlayerContext& Context)
{
	if (!HasAuthority()) return false;
	if (!ThisAsHeldItem) return false;

	//safety check holder comp by seeing if its owner is the same as the instigator
	
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HolderComponentHeldItem = HolderComponent->GetHeldItem();

	if (HolderComponentHeldItem->GetActor() == this) //attach to something else
	{
		HolderComponent->AttachItemToSocket(OwnerMachine->GetMeshComponent(), ParentSocket);

		bInOwnerMachine = true;

		if (HighlightComponent) HighlightComponent->bCanHighlight = true;

		return true;
	}

	return false;
}

bool AHoldableMachinePart::PickUp(const FPlayerContext& Context)
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

		bInOwnerMachine = false;
		
		if (HighlightComponent) HighlightComponent->bCanHighlight = false;

		return true;
	}
	
	return false; //did nothing
}

bool AHoldableMachinePart::Drop(const FPlayerContext& Context)
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

		bInOwnerMachine = false;
		
		if (HighlightComponent) HighlightComponent->bCanHighlight = true;

		return true;
	}

	return false;
}


//Returns
bool AHoldableMachinePart::IsInOwnerMachine()
{
	return bInOwnerMachine;
}


//Replication
void AHoldableMachinePart::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AHoldableMachinePart, ThisAsHeldItem);
}

void AHoldableMachinePart::BeginPlaySetupForReplicatedObjects(UPrimitiveComponent* RootComponentWithPhysics)
{
	if (!HasAuthority()) return;

	if (IsValid(ThisAsHeldItem)) RemoveReplicatedSubObject(ThisAsHeldItem);
	
	ThisAsHeldItem = NewObject<UHeldItem>(this);
	ThisAsHeldItem->Init(this, BaseMeshComp, RootComponentWithPhysics);
	AddReplicatedSubObject(ThisAsHeldItem);
}


//Cosmetic Shiz
// hover
void AHoldableMachinePart::Local_StartHover_Implementation(FPlayerContext Context)
{
	Super::Local_StartHover_Implementation(Context);
	
	if (!ItemPromptComp || !ItemPromptComp->GetPromptBox()) return;
	
	ItemPromptComp->GetPromptBox()->AddPrompts({EAction::PickUp});
	ItemPromptComp->SetVisibility(true);

	if (HighlightComponent) HighlightComponent->EnableHighlight();
}

void AHoldableMachinePart::Local_EndHover_Implementation(FPlayerContext Context)
{
	Super::Local_EndHover_Implementation(Context);
	
	if (!ItemPromptComp || !ItemPromptComp->GetPromptBox()) return;
	
	ItemPromptComp->SetVisibility(false);
	ItemPromptComp->GetPromptBox()->ClearPrompts();

	if (HighlightComponent) HighlightComponent->DisableHighlight();
}