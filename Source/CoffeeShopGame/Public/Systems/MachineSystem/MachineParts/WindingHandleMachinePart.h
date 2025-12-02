#pragma once
#include "CoreMinimal.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "WindingHandleMachinePart.generated.h"


class UContainerComponent;

UCLASS()
class COFFEESHOPGAME_API AWindingHandleMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	//Constructor & Overrides
	AWindingHandleMachinePart();
	virtual void BeginPlay() override;
	virtual bool Init(AMachine* _Owner, FName _ParentSocket) override;
	virtual void Tick(float DeltaTime) override;

protected:
	//Variables --> Editable,
	UPROPERTY(EditAnywhere)
	float EnergyGenerationRate = 100.0;

	UPROPERTY(EditAnywhere)
	float VisualHandleSpinRate = 2.0;
	
	
	//Variables --> Visible, Components
	UPROPERTY(BlueprintReadOnly)
	UContainerComponent* ContainerComponent = nullptr;


	//Variables --> Hidden, Dynamic Vars
	UPROPERTY()
	bool bShouldTickInteraction = false;


	//Methods
	UFUNCTION()
	void TurnOnMachine();

	UFUNCTION()
	void TurnOffMachine();

	UFUNCTION()
	void InteractionTick(float DeltaTime);
	
	
	//Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;
	virtual bool InteractCompleted_Implementation(EActionId ActionId, FInteractionContext Context) override;
};
