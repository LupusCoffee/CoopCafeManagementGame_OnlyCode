#pragma once

#include "CoreMinimal.h"
#include "MachinePart.h"
#include "HoldableMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AHoldableMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	//todo: refactor so it only needs a holdable component to work
	
	// Sets default values for this actor's properties
	AHoldableMachinePart();


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool IsInOwnerMachine();


protected:
	//Variables --> Replicated
	UPROPERTY(Replicated)
	TObjectPtr<UHeldItem> ThisAsHeldItem;

	bool bInOwnerMachine = false;

	
	//Methods --> Interaction Interface
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;

	
	//Methods --> Actual PickUp/Drop Logic
	UFUNCTION()
	virtual bool AttachToParentMachine(const FPlayerContext& Context);
	
	UFUNCTION()
	virtual bool PickUp(const FPlayerContext& Context);

	UFUNCTION()
	virtual bool Drop(const FPlayerContext& Context);

	
	//Methods --> Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	virtual void BeginPlaySetupForReplicatedObjects(UPrimitiveComponent* RootComponentWithPhysics);
};
