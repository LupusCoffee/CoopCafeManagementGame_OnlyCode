#pragma once

#include "CoreMinimal.h"
#include "Systems/MachineSystem/MachineWithHoldableParts.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Components/ContainableComponent.h"
#include "CoffeeGrinderMachine.generated.h"
class UResourceConverterSubsystem;


UCLASS()
class COFFEESHOPGAME_API ACoffeeGrinderMachine : public AMachineWithHoldableParts
{
	GENERATED_BODY()

	
public:
	ACoffeeGrinderMachine();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	

protected:
	//Variables --> Variables, Editable
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MaxGrindProgress = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrindSpeed = 5.0f;

	//Variables --> Variables, Visible
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentGrindProgress = 0.0;
	
	
	//Variables --> Components, Editable
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* PourVfxComp = nullptr;
	
	//Variables --> Components, Hidden
	UPROPERTY(BlueprintReadOnly)
	UContainableComponent* PreGrindedContainer = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UContainableComponent* PostGrindedContainer = nullptr;
	
	//Variables --> Subsystems, Hidden
	UPROPERTY()
	UResourceConverterSubsystem* ResourceConverterSubsystem = nullptr;


	//Methods --> Ticking
	UFUNCTION()
	void TickGrindProgress(float DeltaTime);


	//Cosmetic
	virtual void TurnOn() override;
	virtual void TurnOff() override;
};
