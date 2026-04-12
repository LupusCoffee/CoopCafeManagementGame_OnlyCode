#pragma once
#include "TrackConnectionData.generated.h"

class UTrackSplineComponent;

USTRUCT()
struct FTrackConnectionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTrackSplineComponent* OtherSpline;

	UPROPERTY(EditAnywhere)
	int EndPointIndex;
};