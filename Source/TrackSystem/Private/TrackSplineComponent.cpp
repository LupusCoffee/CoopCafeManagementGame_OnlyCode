// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackSplineComponent.h"

void UTrackSplineComponent::AddConnectedSpline(UTrackSplineComponent* NewSpline, int StartPointIndex, int EndPointIndex)
{
	FConnectionDataContainer& Container = ConnectedSplines.FindOrAdd(StartPointIndex);
	FTrackConnectionData NewConnection;
	NewConnection.OtherSpline = NewSpline;
	NewConnection.EndPointIndex = EndPointIndex;
	Container.Connections.Add(NewConnection);
}

void UTrackSplineComponent::RemoveConnectedSpline(UTrackSplineComponent* SplineToRemove)
{
	TArray<int> RemoveList;
	for(auto& Pair : ConnectedSplines)
	{
		FConnectionDataContainer& Container = Pair.Value;
		Container.Connections.RemoveAll([SplineToRemove](const FTrackConnectionData& Connection)
		{
			return Connection.OtherSpline == SplineToRemove;
		});

		if(Container.Connections.IsEmpty())
			RemoveList.Add(Pair.Key);
	}

	for(auto& Key : RemoveList)
	{
		ConnectedSplines.Remove(Key);
	}
}

bool UTrackSplineComponent::HaveConnectionAtPoint(int PointIndex)
{
	if(ConnectedSplines.Contains(PointIndex))
	{
		return true;
	}

	return false;
}

FConnectionDataContainer* UTrackSplineComponent::GetConnectionDataAtPoint(int PointIndex)
{
	if(ConnectedSplines.Contains(PointIndex))
	{
		return &ConnectedSplines[PointIndex];
	}

	return nullptr;
}

TRACKSYSTEM_API EForwardOrder UTrackSplineComponent::GetForwardOrder()
{
	return IndexOrderOfForwardDirection;
}

void UTrackSplineComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	UWorld* World = GetWorld();
	if (GExitPurge || !World || World->bIsTearingDown)
	{
		return;
	}

	for(auto& Pair : ConnectedSplines)
	{
		FConnectionDataContainer& Container = Pair.Value;
		for(FTrackConnectionData& Connection : Container.Connections)
		{
			if(Connection.OtherSpline)
			{
				Connection.OtherSpline->RemoveConnectedSpline(this);
			}
		}
	}

	ConnectedSplines.Empty();

}

bool UTrackSplineComponent::HaveReachedEnd(int Index)
{
	return (IndexOrderOfForwardDirection == EForwardOrder::Ascending && Index == GetNumberOfSplinePoints() - 1) ||
		   (IndexOrderOfForwardDirection == EForwardOrder::Descending && Index == 0);
}

void UTrackSplineComponent::ShiftIndex(int Index, bool ShiftUpwards)
{
	if (ShiftUpwards)
	{
		
	}
	else
	{

	}
}
