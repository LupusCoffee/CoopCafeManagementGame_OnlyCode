// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "TrackConnectionData.h"
#include "TrackSplineComponent.generated.h"

USTRUCT()
struct FConnectionDataContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FTrackConnectionData> Connections;
};

UENUM()
enum class EForwardOrder : uint8
{
	Ascending,
	Descending
};

/**
 * 
 */
UCLASS(ClassGroup = Utility, ShowCategories = (Mobility), HideCategories = (Physics, Collision, Lighting, Rendering, Mobile), meta = (BlueprintSpawnableComponent), MinimalAPI)
class UTrackSplineComponent : public USplineComponent
{
	GENERATED_BODY()
	
public:
	TRACKSYSTEM_API void AddConnectedSpline(UTrackSplineComponent* NewSpline, int StartPointIndex, int EndPointIndex);
	TRACKSYSTEM_API void RemoveConnectedSpline(UTrackSplineComponent* SplineToRemove);
	TRACKSYSTEM_API bool HaveConnectionAtPoint(int PointIndex);
	TRACKSYSTEM_API FConnectionDataContainer* GetConnectionDataAtPoint(int PointIndex);
	TRACKSYSTEM_API EForwardOrder GetForwardOrder();

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	void SetIsMainSpline(bool bInIsMainSpline) { bIsMainSpline = bInIsMainSpline; }
	bool IsMainSpline() { return bIsMainSpline; }
	bool IsSpawnPoint() { return bIsSpawnPoint; }
	TRACKSYSTEM_API bool HaveReachedEnd(int Index);

	void ShiftIndex(int Index, bool ShiftUpwards = true);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<int, FConnectionDataContainer> ConnectedSplines;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EForwardOrder IndexOrderOfForwardDirection = EForwardOrder::Ascending;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsMainSpline = false;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsSpawnPoint = false;
};
