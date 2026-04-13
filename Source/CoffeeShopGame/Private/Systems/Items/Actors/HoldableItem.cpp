#include "Systems/Items/Actors/HoldableItem.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HoldableComponent.h"


//Core
AHoldableItem::AHoldableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	HoldableComponent = CreateDefaultSubobject<UHoldableComponent>("HoldableComponent");
	
	//Replication
	bReplicates = true;
	AInteractableActor::SetReplicateMovement(true);
	bNetLoadOnClient = true;
	bNetUseOwnerRelevancy = true;
	bReplicateUsingRegisteredSubObjectList = true;
}
