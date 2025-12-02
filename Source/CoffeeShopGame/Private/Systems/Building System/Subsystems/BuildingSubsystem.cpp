// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/Building System/Subsystems/BuildingSubsystem.h"
#include "Core/Data/DataAssets/ItemData.h"

void UBuildingSubsystem::SetupGrid(FVector2D InCellSize, FIntVector2 InGridSize)
{
	CellSize = InCellSize;
	GridSize = InGridSize;
	Tiles.Init(true, GridSize.X * GridSize.Y);

	OnSettingChangedDelegate.Broadcast(CellSize, GridSize);
}

void UBuildingSubsystem::SetTileOnPosition(FIntVector2 TilePosition, bool NewState)
{
	if (!IsPositionValid(TilePosition)) return;

	int Index = TilePosition.Y * GridSize.X + TilePosition.X;

	Tiles[Index] = NewState;

	OnGridUpdatedDelegate.Broadcast(Tiles);
}

void UBuildingSubsystem::SetTileOnLocation(FVector Location, bool NewState)
{
	int X = (Location.X + (CellSize.X / 2)) / CellSize.X;
	int Y = (Location.Y + (CellSize.Y / 2)) / CellSize.Y;

	if (!IsPositionValid(FIntVector2(X, Y)))
		return;

	int Index = Y * GridSize.X + X;
	Tiles[Index] = NewState;

	OnGridUpdatedDelegate.Broadcast(Tiles);
}

bool UBuildingSubsystem::GetTile(FIntVector2 TilePosition)
{
	if (!IsPositionValid(TilePosition)) return false;

	int Index = TilePosition.Y * GridSize.X + TilePosition.X;

	return Tiles[Index];
}

bool UBuildingSubsystem::GetTileAtLocation(FVector Location)
{
	int X = (Location.X + (CellSize.X / 2)) / CellSize.X;
	int Y = (Location.Y + (CellSize.Y / 2)) / CellSize.Y;

	if (!IsPositionValid(FIntVector2(X, Y)))
		return false;

	int Index = Y * GridSize.X + X;

	return Tiles[Index];
}

FVector UBuildingSubsystem::GetClosestLocation(FVector Location)
{
	int X = (Location.X + (CellSize.X / 2)) / CellSize.X;
	int Y = (Location.Y + (CellSize.Y / 2)) / CellSize.Y;

	if (!IsPositionValid(FIntVector2(X, Y)))
		return FVector(-100);

	return FVector((X * CellSize.X), (Y * CellSize.Y), 0);
}

bool UBuildingSubsystem::GetClosestLocation(FVector Location, FVector& OutLocation)
{
	int X = (Location.X + (CellSize.X / 2)) / CellSize.X;
	int Y = (Location.Y + (CellSize.Y / 2)) / CellSize.Y;

	if (!IsPositionValid(FIntVector2(X, Y)))
		return false;

	OutLocation = FVector((X * CellSize.X), (Y * CellSize.Y), 0);
	return true;
}

TArray<FVector2D> UBuildingSubsystem::GetTileLocations()
{
	TArray<FVector2D> ReturnList;

	for (int Y = 0; Y < GridSize.Y; Y++)
	{
		for (int X = 0; X < GridSize.X; X++)
		{
			ReturnList.Add(FVector2D(CellSize.X * X, CellSize.Y * Y));
		}
	}

	return ReturnList;
}

void UBuildingSubsystem::SetTilesWithDataAsset(FVector Location, UFurnitureData* DataAsset, bool IsAvailable)
{
	FIntVector2 Origin((Location.X + (CellSize.X / 2)) / CellSize.X, (Location.Y + (CellSize.Y / 2)) / CellSize.Y);
	TArray<int> Indices;

	for (auto& Offset : DataAsset->Blocks)
	{
		FIntVector2 Position = Origin + Offset;
		if (!IsPositionValid(Position))
			return;

		Indices.Add(Position.Y * GridSize.X + Position.X);
	}

	for (auto& Index : Indices)
	{
		Tiles[Index] = IsAvailable;
	}

	OnGridUpdatedDelegate.Broadcast(Tiles);
}

bool UBuildingSubsystem::CanPlaceFurniture(FVector Location, UFurnitureData* DataAsset)
{
	FIntVector2 Origin((Location.X + (CellSize.X / 2)) / CellSize.X, (Location.Y + (CellSize.Y / 2)) / CellSize.Y);

	for (auto& Offset : DataAsset->Blocks)
	{
		FIntVector2 Position = Origin + Offset;
		if (!IsPositionValid(Position) || !GetTile(Position))
			return false;
	}

	return true;
}

FVector2D UBuildingSubsystem::GetCellSize()
{
	return CellSize;
}

FIntVector2 UBuildingSubsystem::GetGridSize()
{
	return GridSize;
}


bool UBuildingSubsystem::IsPositionValid(FIntVector2 TilePosition)
{
	if (TilePosition.X < 0 || TilePosition.X >= GridSize.X ||
		TilePosition.Y < 0 || TilePosition.Y >= GridSize.Y)
		return false;

	return true;
}
