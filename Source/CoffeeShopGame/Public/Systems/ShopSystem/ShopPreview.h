// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Systems/Items/Actors/HoldableItem.h"
#include "ShopPreview.generated.h"

class UShopManager;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOPGAME_API AShopPreview : public AHoldableItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopPreview();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetupPreview(UShopManager* Manager, UStaticMesh* Mesh, int EntryIndex);

};
