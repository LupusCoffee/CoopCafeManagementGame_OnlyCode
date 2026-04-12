// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseItemData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SHOPSYSTEM_API UBaseItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Properties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString ItemName;

	UPROPERTY(Category = "Properties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString ItemDescription;
};
