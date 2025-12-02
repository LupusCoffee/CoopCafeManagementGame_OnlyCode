// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/Building System/Actors/GridVisualizer.h"
#include "ProceduralMeshComponent.h"
#include "Systems/Building System/Subsystems/BuildingSubsystem.h"
#include "Core/Data/DataAssets/ItemData.h"
#include "Core/Framework/DeveloperSettings/GridSettings.h"

// Sets default values
AGridVisualizer::AGridVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("Parent");

	LineMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LineMesh"));
	LineMesh->SetupAttachment(RootComponent);

	CellMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("CellMesh"));
	CellMesh->SetupAttachment(RootComponent);

	CellPreviewMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PreviewMesh"));
	CellPreviewMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGridVisualizer::BeginPlay()
{
	Super::BeginPlay();

	LineMesh->SetMaterial(0, LineMaterial);
	CellMesh->SetMaterial(0, CellMaterial);
	CellPreviewMesh->SetMaterial(0, PreviewMaterial);

	UBuildingSubsystem* Subsystem = GetWorld()->GetSubsystem<UBuildingSubsystem>();
	if (!Subsystem)
		return;

	Subsystem->OnSettingChangedDelegate.AddDynamic(this, &AGridVisualizer::UpdateMesh);
	Subsystem->OnGridUpdatedDelegate.AddDynamic(this, &AGridVisualizer::UpdateCellMesh);
}

void AGridVisualizer::UpdateMesh(FVector2D NewCellSize, FIntVector2 NewGridSize)
{
	CellSize = NewCellSize;
	GridSize = NewGridSize;

	TArray<FVector> LineVertices;
	TArray<int> LineTriangles;

	// Generate line mesh
	// Row lines
	for (int Row = 0; Row < GridSize.Y + 1; Row++)
	{
		LineTriangles.Add(LineVertices.Num() + 0);
		LineTriangles.Add(LineVertices.Num() + 3);
		LineTriangles.Add(LineVertices.Num() + 2);
		LineTriangles.Add(LineVertices.Num() + 2);
		LineTriangles.Add(LineVertices.Num() + 1);
		LineTriangles.Add(LineVertices.Num() + 0);

		LineVertices.Add(FVector(
			-LineThickness / 2 - (CellSize.X / 2), // X
			CellSize.Y * Row - (LineThickness / 2) - (CellSize.Y / 2), // Y
			0));				 // Z
		LineVertices.Add(FVector(
			CellSize.X * GridSize.X + (LineThickness / 2) - (CellSize.X / 2),
			CellSize.Y * Row - (LineThickness / 2) - (CellSize.Y / 2),
			0));
		LineVertices.Add(FVector(
			CellSize.X * GridSize.X + (LineThickness / 2) - (CellSize.X / 2),
			CellSize.Y * Row + (LineThickness / 2) - (CellSize.Y / 2),
			0));
		LineVertices.Add(FVector(
			-LineThickness / 2 - (CellSize.X / 2),
			CellSize.Y * Row + (LineThickness / 2) - (CellSize.Y / 2),
			0));
	}

	// Column lines
	for (int Col = 0; Col < GridSize.X + 1; Col++)
	{
		LineTriangles.Add(LineVertices.Num() + 0);
		LineTriangles.Add(LineVertices.Num() + 3);
		LineTriangles.Add(LineVertices.Num() + 2);
		LineTriangles.Add(LineVertices.Num() + 2);
		LineTriangles.Add(LineVertices.Num() + 1);
		LineTriangles.Add(LineVertices.Num() + 0);

		LineVertices.Add(FVector(
			CellSize.X * Col - (LineThickness / 2) - (CellSize.X / 2),
			-LineThickness / 2 - (CellSize.Y / 2),
			0));				 
		LineVertices.Add(FVector(
			CellSize.X * Col + (LineThickness / 2) - (CellSize.X / 2),
			-LineThickness / 2 - (CellSize.Y / 2),
			0));
		LineVertices.Add(FVector(
			CellSize.X * Col + (LineThickness / 2) - (CellSize.X / 2),
			CellSize.Y * GridSize.Y + (LineThickness / 2) - (CellSize.Y / 2),
			0));
		LineVertices.Add(FVector(
			CellSize.X * Col - (LineThickness / 2) - (CellSize.X / 2),
			CellSize.Y * GridSize.Y + (LineThickness / 2) - (CellSize.Y / 2),
			0));
	}

	LineMesh->CreateMeshSection(0, LineVertices, LineTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
}

void AGridVisualizer::EnablePreview(bool Enable)
{
	CellPreviewMesh->SetVisibility(Enable);
	CellMesh->SetVisibility(Enable);
	LineMesh->SetVisibility(Enable);
}

void AGridVisualizer::UpdatePreviewMesh(UFurnitureData* FurnitureData)
{
	TArray<FVector> PreviewVertices;
	TArray<int> PreviewTriangles;

	float UseSpacing = FMath::Max(LineThickness, Spacing);

	for (auto& Offset : FurnitureData->Blocks)
	{
		PreviewTriangles.Add(PreviewVertices.Num() + 0);
		PreviewTriangles.Add(PreviewVertices.Num() + 3);
		PreviewTriangles.Add(PreviewVertices.Num() + 2);
		PreviewTriangles.Add(PreviewVertices.Num() + 2);
		PreviewTriangles.Add(PreviewVertices.Num() + 1);
		PreviewTriangles.Add(PreviewVertices.Num() + 0);

		PreviewVertices.Add(FVector(
			(Offset.X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
			(Offset.Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
			0));
		PreviewVertices.Add(FVector(
			(Offset.X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
			(Offset.Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
			0));
		PreviewVertices.Add(FVector(
			(Offset.X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
			(Offset.Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
			0));
		PreviewVertices.Add(FVector(
			(Offset.X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
			(Offset.Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
			0));
	}

	CellPreviewMesh->CreateMeshSection(0, PreviewVertices, PreviewTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
}

void AGridVisualizer::UpdatePreviewMeshLocation(FVector Location)
{
	CellPreviewMesh->SetWorldLocation(FVector(Location.X, Location.Y, GetActorLocation().Z + 0.1f));
}

void AGridVisualizer::UpdatePlacableStatus(bool CanPlace)
{
	CellPreviewMesh->SetVectorParameterValueOnMaterials("PreviewColor", CanPlace ? FVector(0, 1, 0) : FVector(1, 0, 0));
}

// Called every frame
void AGridVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridVisualizer::UpdateCellMesh(const TArray<bool>& Tiles)
{
	TArray<FVector> CellVertices;
	TArray<int> CellTriangles;

	float UseSpacing = FMath::Max(LineThickness, Spacing);

	// Generate cell mesh
	for (int X = 0; X < GridSize.X; X++)
	{
		for (int Y = 0; Y < GridSize.Y; Y++)
		{
			if (Tiles[Y * GridSize.X + X])
				continue;

			CellTriangles.Add(CellVertices.Num() + 0);
			CellTriangles.Add(CellVertices.Num() + 3);
			CellTriangles.Add(CellVertices.Num() + 2);
			CellTriangles.Add(CellVertices.Num() + 2);
			CellTriangles.Add(CellVertices.Num() + 1);
			CellTriangles.Add(CellVertices.Num() + 0);

			CellVertices.Add(FVector(
				(X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
				(Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
				0));
			CellVertices.Add(FVector(
				(X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
				(Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
				0));
			CellVertices.Add(FVector(
				(X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
				(Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
				0));
			CellVertices.Add(FVector(
				(X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
				(Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
				0));
		}
	}

	CellMesh->CreateMeshSection(0, CellVertices, CellTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
}

AEditorGridVisualizer::AEditorGridVisualizer() : AGridVisualizer()
{
	PivotMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PivotMesh"));
	PivotMesh->SetupAttachment(RootComponent);
}

void AEditorGridVisualizer::BeginPlay()
{
	Super::BeginPlay();

	CellSize = UGridSettings::GetGridSettings()->CellSize;

	PivotMesh->SetMaterial(0, PivotMaterial);

	TArray<FVector> CellVertices;
	TArray<int> CellTriangles;

	float UseSpacing = FMath::Max(LineThickness, Spacing);

	CellTriangles.Add(CellVertices.Num() + 0);
	CellTriangles.Add(CellVertices.Num() + 3);
	CellTriangles.Add(CellVertices.Num() + 2);
	CellTriangles.Add(CellVertices.Num() + 2);
	CellTriangles.Add(CellVertices.Num() + 1);
	CellTriangles.Add(CellVertices.Num() + 0);

	CellVertices.Add(FVector(
		-(CellSize.X / 2) + (UseSpacing / 2),
		-(CellSize.Y / 2) + (UseSpacing / 2),
		0));
	CellVertices.Add(FVector(
		+(CellSize.X / 2) - (UseSpacing / 2),
		-(CellSize.Y / 2) + (UseSpacing / 2),
		0));
	CellVertices.Add(FVector(
		+(CellSize.X / 2) - (UseSpacing / 2),
		+(CellSize.Y / 2) - (UseSpacing / 2),
		0));
	CellVertices.Add(FVector(
		-(CellSize.X / 2) + (UseSpacing / 2),
		+(CellSize.Y / 2) - (UseSpacing / 2),
		0));

	PivotMesh->CreateMeshSection(0, CellVertices, CellTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
}

void AEditorGridVisualizer::SetupGrid(FVector InOffset)
{
	Offset = InOffset;

	SetPivotLocation(Pivot);
}

int AEditorGridVisualizer::GetTileIndexAtLocation(FVector Location, FVector2D& CellPosition)
{
	int X = (Location.X + (CellSize.X / 2)) / CellSize.X;
	int Y = (Location.Y + (CellSize.Y / 2)) / CellSize.Y;

	if (X < 0 || X >= GridSize.X ||
		Y < 0 || Y >= GridSize.Y)
		return -1;

	CellPosition = FVector2D(X, Y);

	int Index = Y * GridSize.X + X;
	return Index;
}

int AEditorGridVisualizer::GetTileIndexAtPosition(FVector Location)
{
	return 	Location.Y * GridSize.X + Location.X;;
}

void AEditorGridVisualizer::SetPivotLocation(FVector2D Position)
{
	Pivot = Position;

	FVector Location = FVector(Position.X * CellSize.X, Position.Y * CellSize.Y, 0.0f) + Offset;

	PivotMesh->SetWorldLocation(Location);
}

FVector AEditorGridVisualizer::GetPivotLocation()
{
	return PivotMesh->GetComponentLocation();
}

TArray<FIntVector2> AEditorGridVisualizer::GetBlockData(TArray<bool> Blocks)
{
	TArray<FIntVector2> ReturnList;

	for (int i = 0; i < Blocks.Num(); i++)
	{
		if (Blocks[i])
			ReturnList.Add(FIntVector2(int(i % (int)GridSize.X), int(i / GridSize.X)) - FIntVector2(Pivot.X, Pivot.Y));
	}

	return ReturnList;
}

void AEditorGridVisualizer::UpdateCellMesh(const TArray<bool>& Tiles)
{
	TArray<FVector> CellVertices;
	TArray<int> CellTriangles;

	TArray<FVector> PreviewCellVertices;
	TArray<int> PreviewCellTriangles;

	float UseSpacing = FMath::Max(LineThickness, Spacing);

	// Generate cell mesh
	for (int X = 0; X < GridSize.X; X++)
	{
		for (int Y = 0; Y < GridSize.Y; Y++)
		{
			if (Pivot == FVector2D(X, Y))
				continue;

			if (!Tiles[Y * GridSize.X + X])
			{
				CellTriangles.Add(CellVertices.Num() + 0);
				CellTriangles.Add(CellVertices.Num() + 3);
				CellTriangles.Add(CellVertices.Num() + 2);
				CellTriangles.Add(CellVertices.Num() + 2);
				CellTriangles.Add(CellVertices.Num() + 1);
				CellTriangles.Add(CellVertices.Num() + 0);

				CellVertices.Add(FVector(
					(X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
					(Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
					0));
				CellVertices.Add(FVector(
					(X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
					(Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
					0));
				CellVertices.Add(FVector(
					(X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
					(Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
					0));
				CellVertices.Add(FVector(
					(X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
					(Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
					0));
			}
			else
			{
				PreviewCellTriangles.Add(PreviewCellVertices.Num() + 0);
				PreviewCellTriangles.Add(PreviewCellVertices.Num() + 3);
				PreviewCellTriangles.Add(PreviewCellVertices.Num() + 2);
				PreviewCellTriangles.Add(PreviewCellVertices.Num() + 2);
				PreviewCellTriangles.Add(PreviewCellVertices.Num() + 1);
				PreviewCellTriangles.Add(PreviewCellVertices.Num() + 0);

				PreviewCellVertices.Add(FVector(
					(X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
					(Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
					0));
				PreviewCellVertices.Add(FVector(
					(X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
					(Y * CellSize.Y) - (CellSize.Y / 2) + (UseSpacing / 2),
					0));
				PreviewCellVertices.Add(FVector(
					(X * CellSize.X) + (CellSize.X / 2) - (UseSpacing / 2),
					(Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
					0));
				PreviewCellVertices.Add(FVector(
					(X * CellSize.X) - (CellSize.X / 2) + (UseSpacing / 2),
					(Y * CellSize.Y) + (CellSize.Y / 2) - (UseSpacing / 2),
					0));
			}
		}
	}

	CellMesh->CreateMeshSection(0, CellVertices, CellTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	CellPreviewMesh->CreateMeshSection(0, PreviewCellVertices, PreviewCellTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}
