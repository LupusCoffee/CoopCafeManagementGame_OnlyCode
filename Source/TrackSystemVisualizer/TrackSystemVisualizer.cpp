// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrackSystemVisualizer.h"
#include "TrackSplineComponentVisualizer.h"
#include "TrackSplineComponent.h"
#include "UnrealEdGlobals.h"
#include "Editor.h"
#include "Editor/UnrealEdEngine.h"

IMPLEMENT_GAME_MODULE(FTrackSystemVisualizer, TrackSystemVisualizer);

void FTrackSystemVisualizer::StartupModule()
{
	TrackSplineComponentName = UTrackSplineComponent::StaticClass()->GetFName();

	if (GUnrealEd)
	{
		TSharedPtr<FTrackSplineComponentVisualizer> Visualizer = MakeShared<FTrackSplineComponentVisualizer>();
		GUnrealEd->RegisterComponentVisualizer(TrackSplineComponentName, Visualizer);
		Visualizer->OnRegister();
	}
}

void FTrackSystemVisualizer::ShutdownModule()
{
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(TrackSplineComponentName);
	}
}
