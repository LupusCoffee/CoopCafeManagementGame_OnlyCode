#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "ItemSpotMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AItemSpotMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	AItemSpotMachinePart();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	//Variables --> Editable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Settings")
	bool bUseOwnSocket = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Settings")
	FName OwnSocket = "own_item_socket";
	
	//Variables --> Hidden, Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* SpotCollider = nullptr;
	
	//Variables --> Hidden, Visible
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_ItemAtSpotUpdate)
	TObjectPtr<UHeldItem> ItemAtSpot = nullptr;

	UPROPERTY(Replicated)
	bool HoldingAnItem = false;

	UPROPERTY(Replicated)
	bool SwitchingItems = false;
	
	
	//Methods
	UFUNCTION()
	virtual void MachineActiveUpdate(float DeltaTime);

	UFUNCTION()
	virtual void MachineInactiveUpdate();
	
	UFUNCTION()
	virtual bool PlaceItem(UHolderComponent* HolderComponent);

	UFUNCTION()
	virtual bool TakeItem(UHolderComponent* HolderComponent);

	UFUNCTION()
	virtual bool SwitchItem(UHolderComponent* HolderComponent);

	
	//Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;


	//Methods --> Replication, OnRep
	UFUNCTION()
	void OnRep_ItemAtSpotUpdate(UHeldItem* LastItemAtSpot);
	UFUNCTION()
	void CollisionSettingUponNormalItemInteraction(UHeldItem* LastItemAtSpot);
	UFUNCTION()
	void CollisionSettingUponItemSwitching(UHeldItem* LastItemAtSpot);
	

	//Methods --> Replication, Setup
	//explanation: where you record properties of your actor class that you want to replicate
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()	//explanation: create replicated stuff on Server
	void BeginPlaySetupForReplicatedObjects();
};