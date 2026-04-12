// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "ItemSpawnPoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemPurchased, float, ItemPrice);

UCLASS()
class SHOPSYSTEM_API AItemSpawnPoint : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "ItemSpawnPoint")
	AActor* SpawnedItemActor;

	UPROPERTY()
	UBillboardComponent* Billboard;

	UPROPERTY()
	bool CurrentItemPurchased = false;
	
public:

	UPROPERTY(EditAnywhere, Category = "ItemSpawnPoint")
	float ItemPrice;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FItemPurchased OnItemPurchased;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawnPoint")
	FDataTableRowHandle ShopCategory;

	UPROPERTY(VisibleAnywhere)
	FVector SpawnPointLocation;
	
	UPROPERTY(Replicated, EditAnywhere, Blueprintable, Category = "ItemSpawnPoint")
	TSubclassOf<AActor> CurrentItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemSpawnPoint")
	int ItemHeightOffSet;
	
	// Sets default values for this actor's properties
	AItemSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "ItemSpawnPoint")
	void SpawnItem();

	UFUNCTION(BlueprintImplementableEvent, Category = "ItemSpawnPoint")
	void ItemRestocked();


	//Get/Set Current Item
	UFUNCTION(BlueprintCallable, Category = "ItemSpawnPoint")
	void SetCurrentItem(TSubclassOf<AActor> Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ItemSpawnPoint")
	TSubclassOf<AActor> GetCurrentItem() {return CurrentItem;}

	
	//Item Price Functions
	UFUNCTION(BlueprintCallable, Category = "ItemSpawnPoint")
	void SetItemPrice(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ItemSpawnPoint")
	float GetItemPrice() {return ItemPrice;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ItemSpawnPoint")
	FString GetDisplayItemPrice() {return FString::SanitizeFloat(ItemPrice).Append("$");}

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ItemSpawnPoint")
	AActor* GetSpawnedItem() {return SpawnedItemActor;}

	UFUNCTION(BlueprintCallable, Category = "ItemSpawnPoint")
	AActor* BuyCurrentItem();

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
