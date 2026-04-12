// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Data/DataAssets/ItemData.h"

#include "ShopEntry.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FShopEntry
{
GENERATED_BODY();
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AllowPrivateAccess = true))
	UBaseItemData* ItemData = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	float Price = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	UContentData* ContentData = nullptr;
};
