#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Core/Data/Enums/ActorType.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GameFramework/Actor.h"
#include "HoldableItem.generated.h"

UCLASS()
class COFFEESHOPGAME_API AHoldableItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AHoldableItem();
	virtual void BeginPlay() override;

protected:	
	//Variables --> Exposed
	UPROPERTY(EditAnywhere, Category="Settings|Physics")
	bool bTurnOnPhysics = true;

	UPROPERTY(EditAnywhere, Category="Settings|Physics")
	bool bCanBeThrown = true;
	
	//Variables --> Replicated
	UPROPERTY(Replicated)
	TObjectPtr<UHeldItem> ThisAsHeldItem;
	
	
	//Methods --> Interface
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;
	virtual bool InteractCompleted_Implementation(EActionId ActionId, FInteractionContext Context) override;

	//Methods --> General stuff (these should probably be in a "HoldingComp" or something
	UFUNCTION()
	virtual bool PickUp(const FInteractionContext& Context);

	UFUNCTION()
	virtual bool Drop(const FInteractionContext& Context);

	UFUNCTION()
	virtual bool ThrowStart(FInteractionContext Context);

	UFUNCTION()
	virtual bool ThrowEnd(FInteractionContext Context);
	
	//Methods --> Replication
	//explanation: where you record properties of your actor class that you want to replicate
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()	//explanation: create replicated stuff on Server
	void BeginPlaySetupForReplicatedObjects(UPrimitiveComponent* RootComponentWithPhysics);
};
