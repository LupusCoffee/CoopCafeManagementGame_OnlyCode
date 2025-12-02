// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BuildingSubsystem.generated.h"

/**
 * 
 */

class UFurnitureData;

UCLASS()
class COFFEESHOPGAME_API UBuildingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSettingChangedSignature, FVector2D, NewCellSize, FIntVector2, NewGridSize);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGridUpdatedSignature, const TArray<bool>&, Tiles);
	
public:
	UFUNCTION(BlueprintCallable)
	void SetupGrid(FVector2D InCellSize, FIntVector2 InGridSize);
	
	UFUNCTION(BlueprintCallable)
	void SetTileOnPosition(FIntVector2 TilePosition, bool NewState);

	UFUNCTION(BlueprintCallable)
	void SetTileOnLocation(FVector Location, bool NewState);

	UFUNCTION(BlueprintCallable)
	bool GetTile(FIntVector2 TilePosition);

	UFUNCTION(BlueprintCallable)
	bool GetTileAtLocation(FVector Location);

	UFUNCTION(BlueprintCallable)
	FVector GetClosestLocation(FVector Location);

	bool GetClosestLocation(FVector Location, FVector& OutLocation);

	UFUNCTION(BlueprintCallable)
	TArray<FVector2D> GetTileLocations();

	UFUNCTION(BlueprintCallable)
	void SetTilesWithDataAsset(FVector Location, UFurnitureData* DataAsset, bool IsAvailable);

	UFUNCTION(BlueprintCallable)
	bool CanPlaceFurniture(FVector Location, UFurnitureData* DataAsset);

	FVector2D GetCellSize();
	FIntVector2 GetGridSize();

private:
	bool IsPositionValid(FIntVector2 TilePosition);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSettingChangedSignature OnSettingChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGridUpdatedSignature OnGridUpdatedDelegate;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector2D CellSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FIntVector2 GridSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<bool> Tiles;
};
