// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendedViewportWidget.h"
#include "SceneView.h"
#include "Widgets/SViewport.h"
#include "Slate/SceneViewport.h"
#include "LegacyScreenPercentageDriver.h"
#include "CanvasTypes.h"
#include "EngineModule.h"

#define LOCTEXT_NAMESPACE "EditorUtilityWidgetExtension"

class SExtendedViewport : public SViewport
{
	SLATE_BEGIN_ARGS(SExtendedViewport)
		{
		}
	SLATE_END_ARGS()

	SExtendedViewport()
		: PreviewScene(FPreviewScene::ConstructionValues().SetEditor(false))
	{
	}

	void Construct(const FArguments& InArgs)
	{
		SViewport::FArguments ParentArgs;
		ParentArgs.IgnoreTextureAlpha(false);
		ParentArgs.EnableBlending(false);
		//ParentArgs.RenderDirectlyToWindow(true);
		SViewport::Construct(ParentArgs);

		ViewportClient = MakeShareable(new FCustomViewportClient(&PreviewScene));
		Viewport = MakeShareable(new FSceneViewport(ViewportClient.Get(), SharedThis(this)));

		// The viewport widget needs an interface so it knows what should render
		SetViewportInterface(Viewport.ToSharedRef());
	}

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override
	{
		Viewport->Invalidate();
		//Viewport->InvalidateDisplay();

		Viewport->Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
		ViewportClient->Tick(InDeltaTime);
	}

public:
	TSharedPtr<FCustomViewportClient> ViewportClient;

	TSharedPtr<FSceneViewport> Viewport;

	/** preview scene */
	FPreviewScene PreviewScene;
};

FCustomViewportClient::FCustomViewportClient(FPreviewScene* InPreviewScene) : FUMGViewportClient(InPreviewScene)
{

}

FMatrix FCustomViewportClient::GetCachedMatrix() const
{
	return CachedViewProjectionMatrix;
}


UExtendedViewportWidget::UExtendedViewportWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ShowFlags(ESFIM_Game)
{
	bIsVariable = true;
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		BackgroundColor = FLinearColor::Black;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
		ShowFlags.DisableAdvancedFeatures();
	//ParentArgs.IgnoreTextureAlpha(false);
	//ParentArgs.EnableBlending(true);
	////ParentArgs.RenderDirectlyToWindow(true);
}

void UExtendedViewportWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	ViewportWidget.Reset();
}

TSharedRef<SWidget> UExtendedViewportWidget::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
					.Text(LOCTEXT("Viewport", "Viewport"))
			];
	}
	else
	{
		ViewportWidget = SNew(SExtendedViewport);

		if (GetChildrenCount() > 0)
		{
			ViewportWidget->SetContent(GetContentSlot()->Content ? GetContentSlot()->Content->TakeWidget() : SNullWidget::NullWidget);
		}

		return ViewportWidget.ToSharedRef();
	}
}

void UExtendedViewportWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (ViewportWidget.IsValid())
	{
		check(ViewportWidget->ViewportClient.IsValid());
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
			ViewportWidget->ViewportClient->SetBackgroundColor(BackgroundColor);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
			ViewportWidget->ViewportClient->SetEngineShowFlags(ShowFlags);
	}
}

void UExtendedViewportWidget::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (ViewportWidget.IsValid())
	{
		ViewportWidget->SetContent(InSlot->Content ? InSlot->Content->TakeWidget() : SNullWidget::NullWidget);
	}
}

void UExtendedViewportWidget::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (ViewportWidget.IsValid())
	{
		ViewportWidget->SetContent(SNullWidget::NullWidget);
	}
}

UWorld* UExtendedViewportWidget::GetViewportWorld() const
{
	if (ViewportWidget.IsValid())
	{
		return ViewportWidget->PreviewScene.GetWorld();
	}

	return NULL;
}

FVector UExtendedViewportWidget::GetViewLocation() const
{
	if (ViewportWidget.IsValid())
	{
		return ViewportWidget->ViewportClient->GetViewLocation();
	}

	return FVector();
}

void UExtendedViewportWidget::SetViewLocation(FVector Vector)
{
	if (ViewportWidget.IsValid())
	{
		ViewportWidget->ViewportClient->SetViewLocation(Vector);
	}
}

FRotator UExtendedViewportWidget::GetViewRotation() const
{
	if (ViewportWidget.IsValid())
	{
		return ViewportWidget->ViewportClient->GetViewRotation();
	}

	return FRotator();
}

void UExtendedViewportWidget::SetViewRotation(FRotator Rotator)
{
	if (ViewportWidget.IsValid())
	{
		ViewportWidget->ViewportClient->SetViewRotation(Rotator);
	}
}

AActor* UExtendedViewportWidget::Spawn(TSubclassOf<AActor> ActorClass)
{
	if (ViewportWidget.IsValid())
	{
		UWorld* World = GetViewportWorld();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		return World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	}

	// TODO UMG Report spawning actor error before the world is ready.

	return NULL;
}

void UExtendedViewportWidget::SetShowFlag(FString InShowFlagName, bool InValue)
{
	if (ShowFlags.IsNameThere(*InShowFlagName, 0))
	{
		int32 FlagIndex = ShowFlags.FindIndexByName(*InShowFlagName);
		ShowFlags.SetSingleFlag(FlagIndex, InValue);
		ViewportWidget->ViewportClient->SetEngineShowFlags(ShowFlags);
	}
}

FVector UExtendedViewportWidget::GetCursorWorldDirection(FVector2D CursorPosition)
{
	FVector WorldPosition(0);
    FVector WorldDirection(0);

	FVector2f ViewportPosition = ViewportWidget->Viewport->GetCachedGeometry().GetAbsolutePosition();

	FSceneView::DeprojectScreenToWorld(CursorPosition - (FVector2D)ViewportPosition, FIntRect(0, 0, ViewportWidget->Viewport->GetSizeXY().X, ViewportWidget->Viewport->GetSizeXY().Y), GetViewProjectionMatrix().Inverse(), WorldPosition, WorldDirection);

    return WorldDirection;
}

FMatrix UExtendedViewportWidget::GetViewProjectionMatrix() const
{
	FMatrix ProjectionMatrix = FMatrix::Identity;
	if (ViewportWidget.IsValid())
	{
		check(ViewportWidget->ViewportClient.IsValid());
		ProjectionMatrix = ViewportWidget->ViewportClient->GetCachedMatrix();
	}
	return ProjectionMatrix;
}

void UExtendedViewportWidget::SetEnableAdvancedFeatures(bool InEnableAdvancedFeatures)
{
	ShowFlags.DisableAdvancedFeatures();
	if (ViewportWidget.IsValid())
	{
		check(ViewportWidget->ViewportClient.IsValid());
		if (InEnableAdvancedFeatures)
		{
			ShowFlags.EnableAdvancedFeatures();
			ViewportWidget->ViewportClient->SetEngineShowFlags(ShowFlags);
		}
		else
		{
			ShowFlags.DisableAdvancedFeatures();
			ViewportWidget->ViewportClient->SetEngineShowFlags(ShowFlags);
		}

	}
}

void UExtendedViewportWidget::SetLightIntensity(float InLightIntensity)
{
	ViewportWidget->PreviewScene.SetLightBrightness(InLightIntensity);
}

void UExtendedViewportWidget::SetSkyIntensity(float InLightIntensity)
{
	ViewportWidget->PreviewScene.SetSkyBrightness(InLightIntensity);
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
const FLinearColor& UExtendedViewportWidget::GetBackgroundColor() const
{
	return BackgroundColor;
}

void UExtendedViewportWidget::SetBackgroundColor(const FLinearColor& InColor)
{
	BackgroundColor = InColor;
	if (ViewportWidget.IsValid())
	{
		check(ViewportWidget->ViewportClient.IsValid());
		ViewportWidget->ViewportClient->SetBackgroundColor(BackgroundColor);
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_EDITOR

const FText UExtendedViewportWidget::GetPaletteCategory()
{
	return LOCTEXT("Primitive", "Primitive");
}

#endif

#undef LOCTEXT_NAMESPACE

