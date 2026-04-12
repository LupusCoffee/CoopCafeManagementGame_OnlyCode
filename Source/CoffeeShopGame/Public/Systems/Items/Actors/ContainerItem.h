#pragma once

#include "CoreMinimal.h"
#include "HoldableItem.h"
#include "ContainerItem.generated.h"
class UContainableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPourStateChange);

UCLASS()
class COFFEESHOPGAME_API AContainerItem : public AHoldableItem
{
	GENERATED_BODY()

public:
	AContainerItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UContainableComponent* ContainableComp = nullptr;
};
