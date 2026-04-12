#include "Systems/MachineSystem/MachineParts/ItemSpotMachinePart.h"
#include "Net/UnrealNetwork.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionEnum.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HolderComponent.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/Structs/HeldItem.h"


AItemSpotMachinePart::AItemSpotMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;

	SpotCollider = CreateDefaultSubobject<UBoxComponent>(FName("SpotCollider"));
	SpotCollider->SetCollisionProfileName(TEXT("TraceOverlap"));

	SpotCollider->SetupAttachment(Root);

	//replication
	bReplicateUsingRegisteredSubObjectList = true;
}

void AItemSpotMachinePart::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, *GetName());
}

void AItemSpotMachinePart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerMachine) return;
	
	if (OwnerMachine->IsOn()) MachineActiveUpdate(DeltaTime);
	else MachineInactiveUpdate();
}

void AItemSpotMachinePart::MachineActiveUpdate(float DeltaTime){}
void AItemSpotMachinePart::MachineInactiveUpdate(){}

void AItemSpotMachinePart::Local_StartHover_Implementation(FPlayerContext PlayerContext)
{
	IInteractable::Local_StartHover_Implementation(PlayerContext);

	
	if (HighlightComponent) HighlightComponent->EnableHighlight();
	
	
	if (ItemPromptComp) ItemPromptComp->SetVisibility(true);
}

void AItemSpotMachinePart::Local_TickHover_Implementation(FPlayerContext Context, float DeltaTime)
{
	Super::Local_TickHover_Implementation(Context, DeltaTime);
	
	if (!ItemPromptComp) return;
	
	UHolderComponent* HolderComp = Context.HolderComponent;
	if (!HolderComp) return;
	
	if (HoldingAnItem && !HolderComp->IsHolding())
	{
		ItemPromptComp->GetPromptBox()->SetPrompts({EAction::PickUp});
	}
	else if (!HoldingAnItem && HolderComp->IsHolding())
	{
		ItemPromptComp->GetPromptBox()->SetPrompts({EAction::PlaceDown});
	}
}

void AItemSpotMachinePart::Local_EndHover_Implementation(FPlayerContext Context)
{
	IInteractable::Local_EndHover_Implementation(Context);

	if (HighlightComponent) HighlightComponent->DisableHighlight();
	
	ItemPromptComp->GetPromptBox()->ClearPrompts();
	ItemPromptComp->SetVisibility(false);
}

bool AItemSpotMachinePart::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_StartInteraction_Implementation(ActionId, Context);
	
	
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	
	if (ActionId != EActionId::E) return false;
	
	
	bool PlayerIsHolding = HolderComponent->IsHolding();
	
	if (!ItemAtSpot) PlaceItem(HolderComponent);
	else if (!PlayerIsHolding) TakeItem(HolderComponent);
	else SwitchItem(HolderComponent);

	return true;
}

bool AItemSpotMachinePart::PlaceItem(UHolderComponent* HolderComponent)
{
	if (HoldingAnItem) return false;
	if (!HolderComponent) return false;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return false;

	
	ItemAtSpot = HeldItem;
	HoldingAnItem = true;
	SwitchingItems = false;
	
	if (UStaticMeshComponent* ItemMeshComp = ItemAtSpot->GetMeshComp())
		ItemMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (bUseOwnSocket && BaseMeshComp->GetSocketByName(OwnSocket))
		HolderComponent->AttachItemToSocket(BaseMeshComp, OwnSocket);
	else
		HolderComponent->AttachItemToSocket(OwnerMachine->GetMeshComponent(), ParentSocket);

	return true;
}

bool AItemSpotMachinePart::TakeItem(UHolderComponent* HolderComponent)
{
	if (!ItemAtSpot) return false;
	if (HolderComponent->IsHolding()) return false;

	if (UStaticMeshComponent* ItemMeshComp = ItemAtSpot->GetMeshComp())
	{
		FCollisionResponseContainer InitialColResponses = ItemAtSpot->GetInitialCollisionResponses();
		ItemMeshComp->SetCollisionResponseToChannels(InitialColResponses);
	}
	
	HolderComponent->PickUpItem(ItemAtSpot);	//make it return a bool, and return false if this returns false

	ItemAtSpot = nullptr;
	HoldingAnItem = false;
	SwitchingItems = false;
	
	return true;
}

bool AItemSpotMachinePart::SwitchItem(UHolderComponent* HolderComponent)
{
	if (!ItemAtSpot) return false;

	SwitchingItems = true;

	
	//Attach Held Item
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return false;
	
	if (bUseOwnSocket && BaseMeshComp->GetSocketByName(OwnSocket))
		HolderComponent->AttachItemToSocket(BaseMeshComp, OwnSocket);
	else
		HolderComponent->AttachItemToSocket(OwnerMachine->GetMeshComponent(), ParentSocket);
	
	
	//Pick Up Previous Item At Spot
	if (UStaticMeshComponent* ItemMeshComp = ItemAtSpot->GetMeshComp())
	{
		FCollisionResponseContainer InitialColResponses = ItemAtSpot->GetInitialCollisionResponses();
		ItemMeshComp->SetCollisionResponseToChannels(InitialColResponses);
	}
	
	HolderComponent->PickUpItem(ItemAtSpot);	//make it return a bool, and return false if this returns false


	//Set Item At Spot To What We Were Holding
	ItemAtSpot = HeldItem;
	HoldingAnItem = true;


	//Setting the new item at spot to not have any col
	if (UStaticMeshComponent* ItemMeshComp = ItemAtSpot->GetMeshComp())
		ItemMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	return true;
}


//Replication
// on rep
void AItemSpotMachinePart::OnRep_ItemAtSpotUpdate(UHeldItem* LastItemAtSpot)
{
	if (!SwitchingItems) CollisionSettingUponNormalItemInteraction(LastItemAtSpot);
	else CollisionSettingUponItemSwitching(LastItemAtSpot);
}

void AItemSpotMachinePart::CollisionSettingUponNormalItemInteraction(UHeldItem* LastItemAtSpot)
{
	//null checks
	UHeldItem* HeldItem = nullptr;
	if (ItemAtSpot) HeldItem = ItemAtSpot;
	else HeldItem = LastItemAtSpot;
	
	UStaticMeshComponent* ItemMeshComp = HeldItem->GetMeshComp();
	if (!ItemMeshComp) return;

	//set collision response
	if (HoldingAnItem)
	{
		ItemMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
	else
	{
		FCollisionResponseContainer InitialColResponses = HeldItem->GetInitialCollisionResponses();
		ItemMeshComp->SetCollisionResponseToChannels(InitialColResponses);
	}
}

void AItemSpotMachinePart::CollisionSettingUponItemSwitching(UHeldItem* LastItemAtSpot)
{
	//Item That Was Placed
	UStaticMeshComponent* ItemAtSpotMeshComp = ItemAtSpot->GetMeshComp();
	if (!ItemAtSpotMeshComp) return;

	ItemAtSpotMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);


	//Item That Was Taken
	UStaticMeshComponent* ItemTakenMeshComp = LastItemAtSpot->GetMeshComp();
	if (!ItemTakenMeshComp) return;

	FCollisionResponseContainer InitialColResponses = LastItemAtSpot->GetInitialCollisionResponses();
	ItemTakenMeshComp->SetCollisionResponseToChannels(InitialColResponses);
}


// setup
void AItemSpotMachinePart::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemSpotMachinePart, ItemAtSpot);
	DOREPLIFETIME(AItemSpotMachinePart, HoldingAnItem);
}

void AItemSpotMachinePart::BeginPlaySetupForReplicatedObjects()
{
	//create replicated ItemAtSpot UObject on Server
	
	if (!HasAuthority()) return;

	if (IsValid(ItemAtSpot)) RemoveReplicatedSubObject(ItemAtSpot);
	
	ItemAtSpot = nullptr;
	AddReplicatedSubObject(ItemAtSpot);
}


//Utilities
AActor* AItemSpotMachinePart::GetActorAtSpot()
{
	if (!ItemAtSpot) return nullptr;
	return ItemAtSpot->GetActor();
}