// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "Systems/Items/Actors/HoldableItem.h"
#include "Core/Data/DataAssets/ItemData.h"

#include "Furniture.generated.h"

UCLASS()
class COFFEESHOPGAME_API AFurniture : public AHoldableItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
	UFurnitureData* Data;

public:
	virtual void BeginPlay() override;
	UFurnitureData* GetData();

protected:
	//Methods --> General stuff
	/*virtual bool PickUp(const FPlayerContext& Context) override;
	virtual bool Drop(const FPlayerContext& Context) override;*/

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ResetRotation();
};
