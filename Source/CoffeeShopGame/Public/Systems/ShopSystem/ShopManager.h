// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShopManager.generated.h"

class AShopPreview;
struct FShopEntry;
struct FPlayerContext;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COFFEESHOPGAME_API UShopManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShopManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetupShopArea();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BuyItem(int EntryIndex, FPlayerContext Context);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool CanBuyItem(int EntryIndex);

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<FShopEntry> AvailableItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<AShopPreview*> PreviewActors;
	
		
};
