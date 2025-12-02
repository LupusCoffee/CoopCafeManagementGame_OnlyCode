// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/CoffeeShopTables/CoffeeShopTable.h"
#include "UObject/Object.h"
#include "TableManager.generated.h"

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API ATableManager : public AActor
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category = "Array")
	void AddTable(ACoffeeShopTable* Table);

	UFUNCTION(BlueprintCallable, Category = "Array")
	void RemoveTable(ACoffeeShopTable* Table);

	UFUNCTION(BlueprintCallable, Category = "Array")
	ACoffeeShopTable* GetRandomTable();

	UFUNCTION(BlueprintCallable, Category = "Array")
	ACoffeeShopTable* GetTable(int n);

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TArray<ACoffeeShopTable*> tables;
	
};
