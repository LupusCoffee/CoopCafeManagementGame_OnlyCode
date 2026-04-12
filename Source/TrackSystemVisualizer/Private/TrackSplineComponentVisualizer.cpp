// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackSplineComponentVisualizer.h"
#include "Internationalization/Text.h"
#include "Math/Color.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "EditorViewportClient.h"
#include "Engine/Engine.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Kismet/KismetMathLibrary.h"
#include "PrimitiveDrawingUtils.h"
#include "SceneView.h"
#include "EditorModeManager.h" 
#include "Elements/Framework/TypedElementSelectionSet.h"
#include <Path.h>

#define LOCTEXT_NAMESPACE "SplineComponentVisualizer"

class FTrackSplineComponentVisualizerCommands : public TCommands< FTrackSplineComponentVisualizerCommands>
{
public:
	FTrackSplineComponentVisualizerCommands() : TCommands< FTrackSplineComponentVisualizerCommands>
		(
			"TrackSplineComponentVisualizer",
			LOCTEXT("TrackSplineComponentVisualizer", "TrackSpline Component Visualizer"),
			NAME_None,
			FAppStyle::GetAppStyleSetName()
		)
	{
	}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(AddSubSpline, "Add Subspline", "Add a subspline starting from this point. Click on another point to set the destination of subspline", EUserInterfaceActionType::Button, FInputChord());
	}

public:

	TSharedPtr<FUICommandInfo> AddSubSpline;
};

FTrackSplineComponentVisualizer::FTrackSplineComponentVisualizer() : FSplineComponentVisualizer()
{
	FTrackSplineComponentVisualizerCommands::Register();
}

FTrackSplineComponentVisualizer::~FTrackSplineComponentVisualizer()
{
	FTrackSplineComponentVisualizerCommands::Unregister();
}

void FTrackSplineComponentVisualizer::OnRegister()
{
	FSplineComponentVisualizer::OnRegister();

	const auto& Commands = FTrackSplineComponentVisualizerCommands::Get();

	SplineComponentVisualizerActions->MapAction(
		Commands.AddSubSpline,
		FExecuteAction::CreateSP(this, &FTrackSplineComponentVisualizer::AddSubSpline),
		FCanExecuteAction()
	);
}

void FTrackSplineComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FSplineComponentVisualizer::DrawVisualization(Component, View, PDI);
}

void FTrackSplineComponentVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	const UTrackSplineComponent* TrackSpline = CastChecked<const UTrackSplineComponent>(Component);

	FVector WorldLocation = TrackSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);

	// Project world position to screen
	FVector2D ScreenPosition;
	if (View->WorldToPixel(WorldLocation, ScreenPosition))
	{
		// Load texture once (maybe in constructor or as member variable)
		UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr,
			TEXT("/Game/Tools/Icons/Arrow.Arrow"));

		if (IconTexture)
		{
			const float IconSize = 64.0f;
			FRotator Rotation = TrackSpline->GetRotationAtSplinePoint(1, ESplineCoordinateSpace::World);

				// Adjust for arrow's default pointing direction (assuming arrow points right by default)
				// If your arrow points up by default, use: AngleDegrees - 90.0f

				FCanvasTileItem TileItem(
					ScreenPosition - FVector2D(IconSize * 0.5f, IconSize * 0.5f),
					IconTexture->GetResource(),
					FVector2D(IconSize, IconSize),
					FLinearColor::Blue
				);
				TileItem.BlendMode = SE_BLEND_Translucent;
				TileItem.Rotation = Rotation;
				TileItem.PivotPoint = FVector2D(0.5f, 0.5f);
				Canvas->DrawItem(TileItem);
		}
	}

	FSplineComponentVisualizer::DrawVisualizationHUD(Component, Viewport, View, Canvas);
}

bool FTrackSplineComponentVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click)
{
	ResetTempModes();

	FTypedElementHandle SelectedComponentHandle = VisProxy->GetElementHandle();
	UTypedElementSelectionSet* ElementSelectionSet = (InViewportClient && InViewportClient->GetModeTools()) ? InViewportClient->GetModeTools()->GetEditorSelectionSet() : nullptr;

	if (!ElementSelectionSet->IsElementSelected(SelectedComponentHandle, FTypedElementIsSelectedOptions().SetAllowIndirect(true)))
	{
		GEditor->DeselectAllSurfaces();
		ElementSelectionSet->ClearSelection(FTypedElementSelectionOptions()
			.SetAllowHidden(true)
			.SetWarnIfLocked(true));
		ElementSelectionSet->NotifyPendingChanges();
	}

	bool bVisProxyClickHandled = false;
	UTrackSplineComponent* ClickedComponent = nullptr;

	if (VisProxy && VisProxy->Component.IsValid())
	{
		ClickedComponent = CastChecked<UTrackSplineComponent>(const_cast<UActorComponent*>(VisProxy->Component.Get()));
	}
	else
		return false;

	if (StartParent.IsValid() && StartPointIndex != INDEX_NONE)
	{
		if (FString(VisProxy->GetType()->GetName()) == TEXT("HSplineKeyProxy"))
		{
			ResetTempModes();
			if (ClickedComponent->IsA(UTrackSplineComponent::StaticClass()))
			{
				HSplineKeyProxy* KeyProxy = (HSplineKeyProxy*)VisProxy;

				int EndPointIndex = INDEX_NONE;

				if (ClickedComponent == StartParent.Get())
				{
					if (KeyProxy->KeyIndex != StartPointIndex)
					{
						EndPointIndex = KeyProxy->KeyIndex;

						bVisProxyClickHandled = true;
					}
					else
					{
						StartParent.Reset();
						StartPointIndex = INDEX_NONE;
					}
				}
				else
				{
					EndPointIndex = KeyProxy->KeyIndex;

					bVisProxyClickHandled = true;
				}

				if (bVisProxyClickHandled)
				{
					if (AActor* Owner = StartParent.Get()->GetOwner())
					{
						Owner->Modify();

						int id = Cast<APathMap>(Owner)->GetSubSplineCount();

						UTrackSplineComponent* NewSubspline = NewObject<UTrackSplineComponent>(Owner, *FString("SubSpline" + FString::FromInt(id)));

						Cast<APathMap>(Owner)->IncreaseSubSpline();

						Owner->AddInstanceComponent(NewSubspline);
						NewSubspline->RegisterComponent();
						NewSubspline->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

						int NumOfPoints = NewSubspline->GetNumberOfSplinePoints();
						if (NumOfPoints < 2)
						{
							TArray<FSplinePoint> AdditionalPoints;
							AdditionalPoints.Init(FSplinePoint(), 2 - NumOfPoints);
							NewSubspline->AddPoints(AdditionalPoints, false);
						}

						NewSubspline->SetLocationAtSplinePoint(0, StartParent->GetLocationAtSplinePoint(StartPointIndex, ESplineCoordinateSpace::World), ESplineCoordinateSpace::World, false);
						NewSubspline->SetLocationAtSplinePoint(1, ClickedComponent->GetLocationAtSplinePoint(EndPointIndex, ESplineCoordinateSpace::World), ESplineCoordinateSpace::World, false);
						NewSubspline->UpdateSpline();

						StartParent->AddConnectedSpline(NewSubspline, StartPointIndex, 0);
						NewSubspline->AddConnectedSpline(StartParent.Get(), 0, StartPointIndex);
						ClickedComponent->AddConnectedSpline(NewSubspline, EndPointIndex, 1);
						NewSubspline->AddConnectedSpline(ClickedComponent, 1, EndPointIndex);

						StartParent.Reset();
						StartPointIndex = INDEX_NONE;
					}

					GEditor->RedrawLevelEditingViewports(true);

					return true;
				}
			}
		}
	}

	return FSplineComponentVisualizer::VisProxyHandleClick(InViewportClient, VisProxy, Click);
}

void FTrackSplineComponentVisualizer::GenerateContextMenuSections(FMenuBuilder& InMenuBuilder) const
{
	FSplineComponentVisualizer::GenerateContextMenuSections(InMenuBuilder);

	InMenuBuilder.BeginSection("TrackSpline", LOCTEXT("TrackSpline", "TrackSpline"));
	{
		USplineComponent* SplineComp = GetEditedSplineComponent();
		if (SplineComp != nullptr)
		{
			int SelectedKey = SelectionState->GetLastKeyIndexSelected();
			if (SelectedKey != INDEX_NONE && SelectionState->GetSelectedSegmentIndex() == INDEX_NONE)
			{
				check(SelectedKey >= 0);
				check(SelectedKey < SplineComp->GetNumberOfSplinePoints());
				InMenuBuilder.AddMenuEntry(FTrackSplineComponentVisualizerCommands::Get().AddSubSpline);
			}
		}
	}
	InMenuBuilder.EndSection();
}

void FTrackSplineComponentVisualizer::AddSubSpline()
{
	USplineComponent* SplineComp = Cast<UTrackSplineComponent>(GetEditedSplineComponent());
	int SelectedKey = SelectionState->GetLastKeyIndexSelected();
	if (SelectedKey != INDEX_NONE)
	{
		check(SelectedKey >= 0);
		check(SelectedKey < SplineComp->GetNumberOfSplinePoints());

		StartParent = Cast<UTrackSplineComponent>(SplineComp);
		StartPointIndex = SelectedKey;
	}

	GEditor->RedrawLevelEditingViewports(true);
}