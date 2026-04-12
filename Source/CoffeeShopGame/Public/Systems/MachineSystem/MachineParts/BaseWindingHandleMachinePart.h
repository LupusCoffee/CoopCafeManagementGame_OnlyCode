#pragma once

#include "CoreMinimal.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "BaseWindingHandleMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API ABaseWindingHandleMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	//Constructor & Overrides
	ABaseWindingHandleMachinePart();
	virtual void Tick(float DeltaTime) override;

protected:
	//Variables --> Editable, Static Vars
	UPROPERTY(EditAnywhere)
	float VisualHandleSpinRate = 2.0;


	//Variables --> Hidden, Dynamic Vars
	UPROPERTY()
	bool bShouldTickInteraction = false;
	
	
	//Methods
	UFUNCTION()
	virtual float RotateHandleTick(float DeltaTime);
	
	
	//Interface
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	virtual bool Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
};
