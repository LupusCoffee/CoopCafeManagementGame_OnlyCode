#include "Systems/Items/Actors/ContainerItem.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Components/ContainableComponent.h"


//Setup
AContainerItem::AContainerItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ContainableComp = CreateDefaultSubobject<UContainableComponent>(TEXT("ContainerComp"));
}