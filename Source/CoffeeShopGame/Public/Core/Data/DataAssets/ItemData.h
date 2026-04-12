// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "BaseItemData.h"
#include "ItemData.generated.h"

/**
 * 
 */

struct FResourceAmount;
enum class EResourceType : uint8;

UCLASS(BlueprintType)
class COFFEESHOPGAME_API UContentData : public UBaseItemData
{
	GENERATED_BODY()
	
public:
	// Define additional data here!
	UPROPERTY(Category = "ContentProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UTexture2D* Icon;

	UPROPERTY(Category = "ContentProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EResourceType ResourceType;
};

UCLASS(BlueprintType)
class COFFEESHOPGAME_API UContainerData : public UBaseItemData
{
	GENERATED_BODY()

public:
	// Define additional data here!

	
	//Visuals
	UPROPERTY(Category = "ContainerProperties|Container", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UStaticMesh* Mesh;

	
	//Container
	UPROPERTY(Category = "ContainerProperties|Container", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float MaxVolume;

	UPROPERTY(Category = "ContainerProperties|Container", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), meta = (ToolTip = "How fast content gets removed over time."))
	float DecayVolumeOverTimeRate;

	UPROPERTY(Category = "ContainerProperties|Container", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), meta = (ToolTip = "Resources you can pour into the container."))
	TArray<EResourceType> AcceptedResourceTypes;

	
	//Recipe
	UPROPERTY(Category = "ContainerProperties|Recipe", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bConvertsResourcesToRecipe = false;

	
	//Pouring
	//bCanPour?

	UPROPERTY(Category = "ContainerProperties|Pouring", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), meta = (ToolTip = "Upon pouring into a target, this is the height over which we pour."))
	float HeightOverTargetUponPouringInto;
	
	UPROPERTY(Category = "ContainerProperties|Pouring", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), meta = (ToolTip = "How far down this container pours."))
	float PourDownTraceLength;

	UPROPERTY(Category = "ContainerProperties|Pouring", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float PourDownTraceRadius;

	UPROPERTY(Category = "ContainerProperties|Pouring", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TEnumAsByte<ECollisionChannel> PourDownTraceChannel;
	
	UPROPERTY(Category = "ContainerProperties|Pouring", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), meta = (ToolTip = "The resource that pours out from this container."))
	EResourceType PouringResource;
};

UCLASS(BlueprintType)
class COFFEESHOPGAME_API UFurnitureData : public UBaseItemData
{
	GENERATED_BODY()

public:
	// Define additional data here!

	UPROPERTY(Category = "FurnitureProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UStaticMesh* Mesh;

	UPROPERTY(Category = "FurnitureProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UTexture2D* Thumbnail;

	UPROPERTY(Category = "FurnitureProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector Location = FVector::ZeroVector;

	UPROPERTY(Category = "FurnitureProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector Scale = FVector::OneVector;

	UPROPERTY(Category = "FurnitureProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FIntVector2 Pivot;

	UPROPERTY(Category = "FurnitureProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<FIntVector2> Blocks;

	UPROPERTY(Category = "FurnitureProperties", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector OffsetFromOrigin;
};