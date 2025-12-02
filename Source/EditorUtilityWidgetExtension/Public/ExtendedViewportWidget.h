// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Viewport.h"
#include "ExtendedViewportWidget.generated.h"

/**
 * 
 */

class FCustomViewportClient : public FUMGViewportClient
{
public:
	FCustomViewportClient(FPreviewScene* InPreviewScene = nullptr);

	FMatrix GetCachedMatrix() const;

private:
	FViewport* StoredViewport;
};

UCLASS(MinimalAPI)
class UExtendedViewportWidget : public UContentWidget
{
	GENERATED_BODY()

	UE_DEPRECATED(5.2, "Direct access to BackgroundColor is deprecated. Please use the getter or setter.")
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = Appearance, meta = (AllowPrivateAccess = true))
	FLinearColor BackgroundColor;

	UFUNCTION(BlueprintCallable, Category = "Viewport")
	UWorld* GetViewportWorld() const;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	FVector GetViewLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetViewLocation(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	FRotator GetViewRotation() const;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetViewRotation(FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = "Viewport")
	AActor* Spawn(TSubclassOf<AActor> ActorClass);

	void SetBackgroundColor(const FLinearColor& InColor);

	const FLinearColor& GetBackgroundColor() const;

	UFUNCTION(BlueprintCallable, Category = "Viewport")
	void SetEnableAdvancedFeatures(bool InEnableAdvancedFeatures);

	UFUNCTION(BlueprintCallable, Category = "Viewport")
	void SetLightIntensity(float LightIntensity);

	UFUNCTION(BlueprintCallable, Category = "Viewport")
	void SetSkyIntensity(float LightIntensity);

	UFUNCTION(BlueprintCallable, Category = "Viewport")
	void SetShowFlag(FString InShowFlagName, bool InValue);

	UFUNCTION(BlueprintCallable, Category = "Viewport")
	FVector GetCursorWorldDirection(FVector2D CursorPosition);


	// Cannot find symbols for some reason, I have UMG in dependencies
	UFUNCTION(BlueprintCallable, Category = "Viewport")
	FMatrix GetViewProjectionMatrix() const;


	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

public:
	UExtendedViewportWidget(const FObjectInitializer& ObjectInitializer);

protected:

	// UPanelWidget
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

protected:
	TSharedPtr<class SExtendedViewport> ViewportWidget;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	/** Show flags for the engine for this viewport */
	FEngineShowFlags ShowFlags;
};
