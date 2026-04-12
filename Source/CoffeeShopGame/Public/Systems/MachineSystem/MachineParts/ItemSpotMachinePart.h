#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "ItemSpotMachinePart.generated.h"

UCLASS(Blueprintable)
class COFFEESHOPGAME_API AItemSpotMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	AItemSpotMachinePart();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetActorAtSpot();

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

	
	//Interface
	virtual void Local_StartHover_Implementation(FPlayerContext PlayerContext) override;
	virtual void Local_TickHover_Implementation(FPlayerContext Context, float DeltaTime) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;

	
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
