#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "HoldableComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UHoldableComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:
	UHoldableComponent();
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	
protected:
	//Variables --> Editable
	UPROPERTY(EditAnywhere, Category="Settings|Physics")
	bool bCanBeThrown = true;
	
	//Variables --> Hidden
	UPROPERTY(Replicated)
	TObjectPtr<UHeldItem> ThisAsHeldItem;
	
	//Variables --> Hidden, Components & Actors
	UPROPERTY()
	AActor* Owner;
	
	UPROPERTY()
	UItemPromptComponent* ItemPromptComp = nullptr;
	
	
	//Methods --> Setup
	UFUNCTION()
	void OwnerAsHeldItemSetup();
	
	//Methods --> Logic
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	virtual bool Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	
	UFUNCTION()
	virtual bool PickUp(const FPlayerContext& PlayerContext);

	UFUNCTION()
	virtual bool Drop(const FPlayerContext& PlayerContext);

	UFUNCTION()
	virtual bool ThrowStart(FPlayerContext PlayerContext);

	UFUNCTION()
	virtual bool ThrowEnd(FPlayerContext PlayerContext);
	
	//Methods --> Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Methods --> Visuals
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
};
