// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridVisualizer.generated.h"

class UProceduralMeshComponent;
class UFurnitureData;

UCLASS()
class COFFEESHOPGAME_API AGridVisualizer : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UProceduralMeshComponent* LineMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UProceduralMeshComponent* CellMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UProceduralMeshComponent* CellPreviewMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float LineThickness;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMaterial* LineMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Spacing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMaterial* CellMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMaterial* PreviewMaterial;

	FVector2D CellSize;
	FIntVector2 GridSize;
	
public:	
	// Sets default values for this actor's properties
	AGridVisualizer();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void UpdateCellMesh(const TArray<bool>& Tiles);

	UFUNCTION(BlueprintCallable)
	void EnablePreview(bool Enable);

	UFUNCTION(BlueprintCallable)
	void UpdatePreviewMesh(UFurnitureData* FurnitureData);

	UFUNCTION(BlueprintCallable)
	void UpdatePreviewMeshLocation(FVector Location);

	UFUNCTION(BlueprintCallable)
	void UpdatePlacableStatus(bool CanPlace);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION(BlueprintCallable)
	void UpdateMesh(FVector2D NewCellSize, FIntVector2 NewGridSize);


public:	

};

UCLASS()
class AEditorGridVisualizer : public AGridVisualizer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UProceduralMeshComponent* PivotMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMaterial* PivotMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector2D Pivot = FVector2D(2);
	FVector Offset;

public:
	AEditorGridVisualizer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetupGrid(FVector InOffset);

	UFUNCTION(BlueprintCallable)
	int GetTileIndexAtLocation(FVector Location, FVector2D& CellPosition);

	UFUNCTION(BlueprintCallable)
	int GetTileIndexAtPosition(FVector Location);

	UFUNCTION(BlueprintCallable)
	void SetPivotLocation(FVector2D Position);

	UFUNCTION(BlueprintCallable)
	FVector GetPivotLocation();

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector2> GetBlockData(TArray<bool> Blocks);

	void UpdateCellMesh(const TArray<bool>& Tiles) override;
};