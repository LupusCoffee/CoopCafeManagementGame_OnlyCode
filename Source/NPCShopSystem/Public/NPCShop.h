// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "ItemSpawnPoint.h"
#include "GameFramework/Actor.h"
#include "ShopStockStruct.h"
#include "NPCShop.generated.h"

USTRUCT(BlueprintType)
struct FItemArrayWrapper
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TArray<FItemStruct> Items;
};


UCLASS(Blueprintable)
class SHOPSYSTEM_API ANPCShop : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FName> RowNames;
	UPROPERTY(VisibleAnywhere)
	TArray<TSubclassOf<AActor>> YesterdaysItems;
	UPROPERTY(VisibleAnywhere)
	TArray<TSubclassOf<AActor>> TodaysItems;
	UPROPERTY(VisibleAnywhere)
	TMap<FName, FItemArrayWrapper> ActiveItemPool;

	UPROPERTY(VisibleAnywhere)
	float TotalMoneySpentAtStore;
	UPROPERTY(VisibleAnywhere)
	float TotalMoneyDonated;


	enum PriceRangesEnum
	{
		Low = 0,
		Medium = 1,
		High = 2
	};
	
	
	UFUNCTION(BlueprintCallable)
	void InnitialSetup();

	UFUNCTION()
	void RotateStock();

	UFUNCTION()
	void SpawnRandomItem(AItemSpawnPoint* SpawnPoint);

	UFUNCTION()
	void SubscribeToEvents();
	
public:	
	// Sets default values for this actor's properties
	ANPCShop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AItemSpawnPoint*> StaticItemSpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AItemSpawnPoint*> RotatingItemSpawnPoints;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RestockItems();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void SaveCurrentStock();

	UFUNCTION()
	void IncreaseStoreFunds(float MoneySpent);

	UFUNCTION(BlueprintCallable)
	void DonateMoneyToStore(float MoneyDonated);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shop")
	float GetMoneySpentAtStore() {return TotalMoneySpentAtStore;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shop")
	float GetStoreDonations() {return TotalMoneyDonated;}
	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
