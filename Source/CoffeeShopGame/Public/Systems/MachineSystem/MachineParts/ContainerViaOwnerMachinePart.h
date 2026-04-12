#pragma once

#include "CoreMinimal.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Components/ContainableComponent.h"
#include "ContainerViaOwnerMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AContainerViaOwnerMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	//Public Overrides / Constructor
	AContainerViaOwnerMachinePart();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UContainableComponent* GetContainerComp();
	

protected:	
	//Variables --> Visible, Components	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UContainableComponent* ContainerComponent = nullptr;
	

	//Interface
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
};
