#pragma once
#include "CoreMinimal.h"
#include "BaseWindingHandleMachinePart.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Components/ContainableComponent.h"
#include "WindingHandleEnergyGenerationMachinePart.generated.h"


class UContainerComponent;

UCLASS()
class COFFEESHOPGAME_API AWindingHandleEnergyGenerationMachinePart : public ABaseWindingHandleMachinePart
{
	GENERATED_BODY()

public:
	//Constructor & Overrides
	AWindingHandleEnergyGenerationMachinePart();
	virtual void BeginPlay() override;
	virtual bool Init(AMachine* _Owner, FName _ParentSocket) override;

protected:
	//Variables --> Editable,
	UPROPERTY(EditAnywhere)
	float EnergyGenerationRate = 100.0;
	
	
	//Variables --> Visible, Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UContainableComponent* ContainableComponent = nullptr;


	//Methods
	UFUNCTION()
	void TurnOnMachine();

	UFUNCTION()
	void TurnOffMachine();
	
	virtual float RotateHandleTick(float DeltaTime) override;
};
