// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HitProxies.h"
#include "InputCoreTypes.h"

#include "ComponentVisualizer.h"
#include "SplineComponentVisualizer.h"

#include "Components/SplineComponent.h"
#include "TrackSplineComponent.h"

class FUICommandList;

/**
 * 
 */
class FTrackSplineComponentVisualizer : public FSplineComponentVisualizer
{
public:
	FTrackSplineComponentVisualizer();
	~FTrackSplineComponentVisualizer();

	virtual void OnRegister() override;
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	virtual void GenerateContextMenuSections(FMenuBuilder& InMenuBuilder) const;

protected:
	void AddSubSpline();

private:
	UPROPERTY()
	TWeakObjectPtr<UTrackSplineComponent> StartParent;
	int StartPointIndex = INDEX_NONE;
};
