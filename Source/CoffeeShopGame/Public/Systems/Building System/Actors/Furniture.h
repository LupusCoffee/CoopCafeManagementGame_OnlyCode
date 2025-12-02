// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "Systems/Interaction System/Components/HighlightComponent.h"
#include "Systems/Items/Actors/HoldableItem.h"
#include "Core/Data/DataAssets/ItemData.h"

#include "Furniture.generated.h"

UCLASS()
class COFFEESHOPGAME_API AFurniture : public AHoldableItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UFurnitureData* Data;

public:
	virtual void BeginPlay() override;
	UFurnitureData* GetData();

protected:
	//Methods --> General stuff
	virtual bool PickUp(const FInteractionContext& Context) override;
	virtual bool Drop(const FInteractionContext& Context) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ResetRotation();
};
