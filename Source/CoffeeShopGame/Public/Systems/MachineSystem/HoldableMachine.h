#pragma once

#include "CoreMinimal.h"
#include "Systems/MachineSystem/Machine.h"
#include "HoldableMachine.generated.h"


UCLASS()
class COFFEESHOPGAME_API AHoldableMachine : public AMachine
{
	GENERATED_BODY()

public:
	AHoldableMachine();
	virtual void BeginPlay() override;


protected:
	//Variables --> Replicated
	UPROPERTY(Replicated)
	TObjectPtr<UHeldItem> ThisAsHeldItem;
	
	
	//Methods --> Hover Interface
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;

	
	//Methods --> Interaction Interface
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;

	
	//Methods --> Actual PickUp/Drop Logic
	UFUNCTION()
	virtual bool PickUp(const FPlayerContext& Context);

	UFUNCTION()
	virtual bool Drop(const FPlayerContext& Context);

	
	//Methods --> Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void BeginPlaySetupForReplicatedObjects(UPrimitiveComponent* RootComponentWithPhysics);
};
