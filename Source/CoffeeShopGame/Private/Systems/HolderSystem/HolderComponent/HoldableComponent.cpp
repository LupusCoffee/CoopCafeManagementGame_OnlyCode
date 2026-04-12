#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HoldableComponent.h"
#include "Net/UnrealNetwork.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionEnum.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptHelper.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"


//Setup
UHoldableComponent::UHoldableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	bReplicateUsingRegisteredSubObjectList = true;
}

void UHoldableComponent::OnRegister()
{
	Super::OnRegister();

	//Owner Actor
	Owner = GetOwner();
	if (!Owner) return;
	
	if (!Owner->GetIsReplicated()) Owner->SetReplicates(true);
	if (!Owner->IsReplicatingMovement()) Owner->SetReplicateMovement(true);

	//Owner Mesh
	UStaticMeshComponent* Mesh = Owner->GetComponentByClass<UStaticMeshComponent>();
	if (!Mesh) return;

	Mesh->SetIsReplicated(true);
}

void UHoldableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//Null Checks
	if (!Owner) return;
	
	//Item Prompts
	ItemPromptComp = PromptHelper::GetOrCreateItemPromptComponent(Owner);

	//Setup
	OwnerAsHeldItemSetup();
}

void UHoldableComponent::OwnerAsHeldItemSetup()
{
	if (!Owner) return;
	if (!Owner->HasAuthority()) return;

	//Get Root Prim
	UStaticMeshComponent* Mesh = Owner->GetComponentByClass<UStaticMeshComponent>();
	if (!Mesh) return;
	
	UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
	if (!RootPrim) RootPrim = Mesh;

	//Setup Held item
	if (IsValid(ThisAsHeldItem)) RemoveReplicatedSubObject(ThisAsHeldItem);
	
	ThisAsHeldItem = NewObject<UHeldItem>(Owner);
	ThisAsHeldItem->Init(Owner, Mesh, RootPrim);
	AddReplicatedSubObject(ThisAsHeldItem);
}


//Logic
bool UHoldableComponent::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	IInteractable::Server_StartInteraction_Implementation(ActionId, Context);
	
	if (ActionId == EActionId::Q && bCanBeThrown) return ThrowStart(Context);
	
	if (ActionId == EActionId::E)
	{
		if (PickUp(Context)) return true;
		if (Drop(Context)) return true;
	}

	return false;
}

bool UHoldableComponent::Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	IInteractable::Server_EndInteraction_Implementation(ActionId, Context);

	if (ActionId == EActionId::Q && bCanBeThrown) return ThrowEnd(Context);

	return false;
}

bool UHoldableComponent::PickUp(const FPlayerContext& PlayerContext)
{
	if (!Owner->HasAuthority()) return false;
	if (!ThisAsHeldItem) return false;
	
	UHolderComponent* HolderComponent = PlayerContext.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HolderComponentHeldItem = HolderComponent->GetHeldItem();

	if (!HolderComponentHeldItem) //pick up
	{
		HolderComponent->PickUpItem(ThisAsHeldItem);

		return true;
	}
	
	return false; //did nothing
}

bool UHoldableComponent::Drop(const FPlayerContext& PlayerContext)
{
	if (!Owner->HasAuthority()) return false;
	if (!ThisAsHeldItem) return false;

	UHolderComponent* HolderComponent = PlayerContext.HolderComponent;
	if (!HolderComponent) return false;

	UHeldItem* HolderComponentHeldItem = HolderComponent->GetHeldItem();

	if (HolderComponentHeldItem->GetActor() == Owner) //drop
	{
		HolderComponent->DropItem(PlayerContext.LookedAtLocation);

		return true;
	}

	return false;
}

bool UHoldableComponent::ThrowStart(FPlayerContext PlayerContext)
{
	UHolderComponent* HolderComponent = PlayerContext.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return false;
	
	AActor* HeldItemActor = HeldItem->GetActor();
	if (HeldItemActor != Owner) return false;

	HolderComponent->ServerSide_ThrowItemStart();

	return true;
}

bool UHoldableComponent::ThrowEnd(FPlayerContext PlayerContext)
{
	UHolderComponent* HolderComponent = PlayerContext.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return false;
	
	AActor* HeldItemActor = HeldItem->GetActor();
	if (HeldItemActor != Owner) return false;

	PlayerContext.HolderComponent->ServerSide_ThrowItemCompleted(PlayerContext.InteractorController);

	return true;
}


//Replication
void UHoldableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UHoldableComponent, ThisAsHeldItem);
}


//Visuals
void UHoldableComponent::Local_StartHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_StartHover_Implementation(Context);
	
	if (!ItemPromptComp) return;
	
	ItemPromptComp->GetPromptBox()->AddPrompts({EAction::PickUp});
	ItemPromptComp->SetVisibility(true);
}

void UHoldableComponent::Local_EndHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_EndHover_Implementation(Context);
	
	if (!ItemPromptComp) return;
	
	ItemPromptComp->SetVisibility(false);
	ItemPromptComp->GetPromptBox()->RemovePrompts({EAction::PickUp});
}

