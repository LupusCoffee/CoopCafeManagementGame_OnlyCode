// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOAPSystem/GOAP/Components/GoapComponent.h"
#include "AIDebugComponent.generated.h"

class UGoapComponent;
enum class EGoapAgentState : uint8;


UCLASS(ClassGroup=(AIComponent), meta=(BlueprintSpawnableComponent))
class GOAPSYSTEM_API UAIDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIDebugComponent();
	
	//---------------------LIFECYCLE---------------------//
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	
	//---------------------DEBUG FUNCTIONS---------------------//
	//Visualization
	
	/** Enable/disable debug visualization */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void SetVisualizationEnabled(bool bEnabled);
    
	/** Get current visualization state */
	UFUNCTION(BlueprintPure, Category = "AI Debug")
	bool IsVisualizationEnabled() const { return bDrawDebug; }
    
	/** Draw current AI state */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void DrawCurrentState();
	
	//History
	
	/** Start recording position history */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void StartHistoryRecording();
    
	/** Stop recording position history */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void StopHistoryRecording();
    
	/** Clear position history */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void ClearHistory();
    
	/** Get position history */
	UFUNCTION(BlueprintPure, Category = "AI Debug")
	const TArray<FVector>& GetPositionHistory() const { return PositionHistory; }
	
	//Bookmark
	
	/** Create a bookmark of current state */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void CreateBookmark(const FString& BookmarkName);
    
	/** Restore from a bookmark */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	bool RestoreFromBookmark(const FString& BookmarkName);
    
	/** List all bookmarks */
	UFUNCTION(BlueprintPure, Category = "AI Debug")
	TArray<FString> GetBookmarkNames() const;
	
	//Telemetry
	
	/** Get total distance traveled */
	UFUNCTION(BlueprintPure, Category = "AI Debug")
	float GetTotalDistanceTraveled() const { return TotalDistanceTraveled; }
    
	/** Get time in current state */
	UFUNCTION(BlueprintPure, Category = "AI Debug")
	float GetTimeInCurrentState() const;
    
	/** Get actions completed count */
	UFUNCTION(BlueprintPure, Category = "AI Debug")
	int32 GetActionsCompleted() const { return ActionsCompleted; }
	
	/** Increment actions completed (called when GOAP action finishes) */
	UFUNCTION(BlueprintCallable, Category = "AI Debug")
	void IncrementActionsCompleted() { ActionsCompleted++; }
	
	//Configuration
	
	/** Whether to draw debug visualization */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = false;
    
	/** Color for debug drawing */
	UPROPERTY(EditAnywhere, Category = "Debug")
	FColor DebugColor = FColor::Cyan;
    
	/** Maximum history entries */
	UPROPERTY(EditAnywhere, Category = "Debug")
	int32 MaxHistoryEntries = 1000;
	
	/** Auto-start history recording on begin play */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bAutoStartRecording = true;
	
	/** Show detailed GOAP information */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowGoapInfo = true;
	
	/** Font scale for debug text */
	UPROPERTY(EditAnywhere, Category = "Debug")
	float TextScale = 1.5f;
	
private:
	//---------------------DEBUG DATA---------------------//
	
	/** Position history for path visualization */
	TArray<FVector> PositionHistory;
    
	/** State bookmarks */
	TMap<FString, FTransform> Bookmarks;
    
	/** Telemetry data */
	float TotalDistanceTraveled = 0.0f;
	float StateStartTime = 0.0f;
	int32 ActionsCompleted = 0;
	FVector LastPosition = FVector::ZeroVector;
    
	/** Whether currently recording */
	bool bRecordingHistory = false;
	
	/** Update position history */
	void UpdateHistory();

	/** Bind to GOAP observer updates when the component becomes available */
	void TryBindGoapObserver();

	/** Reset state timer when observed GOAP state changes */
	UFUNCTION()
	void HandleObservedGoapStateChanged();
    
	/** Draw debug info on screen */
	void DrawDebugInfo();
	
	/** Get formatted state text for display */
	FString GetCurrentStateText() const;

	UPROPERTY()
	UGoapComponent* CachedGoapComponent = nullptr;

	EGoapAgentState LastObservedState = EGoapAgentState::Idle;
	bool bGoapObserverBound = false;
};
