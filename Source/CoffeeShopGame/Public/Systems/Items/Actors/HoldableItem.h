#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "GameFramework/Actor.h"
#include "HoldableItem.generated.h"
class UHoldableComponent;

UCLASS()
class COFFEESHOPGAME_API AHoldableItem : public AInteractableActor
{
	GENERATED_BODY()

public:
	AHoldableItem();

protected:	
	//Variables --> Exposed
	UPROPERTY(EditAnywhere)
	UHoldableComponent* HoldableComponent = nullptr;
};
