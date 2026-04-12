// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ShopStockStruct.generated.h"

USTRUCT()
struct FItemStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Price;
};

USTRUCT()
struct FShopPriceRanges : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemStruct> LowRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemStruct> MidRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemStruct> HighRange;
};

USTRUCT(BlueprintType)
struct FShopCatalog : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FShopPriceRanges> PriceRanges;
};
