// Fill out your copyright notice in the Description page of Project Settings.


#include "AIDebugComponent.h"
#include "GOAPSystem/GOAP/Components/GoapComponent.h"
#include "GOAPSystem/AI/Utils/AIComponentUtils.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UAIDebugComponent::UAIDebugComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIDebugComponent::BeginPlay()
{
	Super::BeginPlay();
	TryBindGoapObserver();
	
	// Initialize state time tracking
	if (UWorld* World = GetWorld())
	{
		StateStartTime = World->GetTimeSeconds();
	}
	
	// Initialize last position
	if (AActor* Owner = GetOwner())
	{
		LastPosition = Owner->GetActorLocation();
	}
	
	// Auto-start recording if enabled
	if (bAutoStartRecording && bDrawDebug)
	{
		StartHistoryRecording();
		UE_LOG(LogTemp, Log, TEXT("Auto-started history recording for %s"), 
			*GetOwner()->GetName());
	}
}

void UAIDebugComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bGoapObserverBound && CachedGoapComponent)
	{
		CachedGoapComponent->OnObservedStateChanged.RemoveDynamic(this, &UAIDebugComponent::HandleObservedGoapStateChanged);
	}

	bGoapObserverBound = false;
	CachedGoapComponent = nullptr;

	Super::EndPlay(EndPlayReason);
}

void UAIDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
	if (bDrawDebug)
	{
		DrawDebugInfo();
	}

	if (!bGoapObserverBound)
	{
		TryBindGoapObserver();
	}
    
	if (bRecordingHistory)
	{
		UpdateHistory();
	}
}

void UAIDebugComponent::SetVisualizationEnabled(bool bEnabled)
{
	bDrawDebug = bEnabled;
	
	if (bEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("Debug visualization enabled for %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Debug visualization disabled for %s"), *GetOwner()->GetName());
	}
}

void UAIDebugComponent::DrawCurrentState()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	FVector Location = Owner->GetActorLocation();
	FRotator Rotation = Owner->GetActorRotation();
	
	// Draw position sphere
	DrawDebugSphere(World, Location, 100.0f, 12, DebugColor, false, 0, 0, 3.0f);
	
	// Draw forward direction
	FVector Forward = Rotation.Vector() * 200.0f;
	DrawDebugDirectionalArrow(World, Location, Location + Forward, 50.0f, DebugColor, false, -1.0f, 0, 3.0f);
	
	// Draw info text
	FString InfoText = FString::Printf(TEXT("%s\nDist: %.1f\nActions: %d"), 
		*Owner->GetName(), 
		TotalDistanceTraveled, 
		ActionsCompleted);
	DrawDebugString(World, Location + FVector(0, 0, 150), InfoText, nullptr, DebugColor, -1.0f, false);
}

void UAIDebugComponent::StartHistoryRecording()
{
	if (!bRecordingHistory)
	{
		bRecordingHistory = true;
		LastPosition = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
		UE_LOG(LogTemp, Log, TEXT("Started history recording for %s"), 
			GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
	}
}

void UAIDebugComponent::StopHistoryRecording()
{
	if (bRecordingHistory)
	{
		bRecordingHistory = false;
		UE_LOG(LogTemp, Log, TEXT("Stopped history recording for %s (Recorded %d points)"), 
			GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"), 
			PositionHistory.Num());
	}
}

void UAIDebugComponent::ClearHistory()
{
	PositionHistory.Empty();
	TotalDistanceTraveled = 0.0f;
	LastPosition = FVector::ZeroVector;
	
	UE_LOG(LogTemp, Log, TEXT("Cleared history for %s"), 
		GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
}

void UAIDebugComponent::CreateBookmark(const FString& BookmarkName)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot create bookmark - no owner"));
		return;
	}
	
	FTransform CurrentTransform = Owner->GetActorTransform();
	Bookmarks.Add(BookmarkName, CurrentTransform);
	
	UE_LOG(LogTemp, Log, TEXT("Created bookmark '%s' for %s at %s"), 
		*BookmarkName, 
		*Owner->GetName(), 
		*CurrentTransform.GetLocation().ToString());
}

bool UAIDebugComponent::RestoreFromBookmark(const FString& BookmarkName)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot restore bookmark - no owner"));
		return false;
	}
	
	FTransform* SavedTransform = Bookmarks.Find(BookmarkName);
	if (!SavedTransform)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bookmark '%s' not found"), *BookmarkName);
		return false;
	}
	
	Owner->SetActorTransform(*SavedTransform);
	
	UE_LOG(LogTemp, Log, TEXT("Restored bookmark '%s' for %s to %s"), 
		*BookmarkName, 
		*Owner->GetName(), 
		*SavedTransform->GetLocation().ToString());
	
	return true;
}

TArray<FString> UAIDebugComponent::GetBookmarkNames() const
{
	TArray<FString> Names;
	Bookmarks.GetKeys(Names);
	return Names;
}

float UAIDebugComponent::GetTimeInCurrentState() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}
	
	return World->GetTimeSeconds() - StateStartTime;
}

void UAIDebugComponent::UpdateHistory()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
    
	FVector CurrentPosition = Owner->GetActorLocation();
	
	if (!LastPosition.IsZero())
	{
		TotalDistanceTraveled += FVector::Dist(LastPosition, CurrentPosition);
	}
    
	LastPosition = CurrentPosition;
	
	PositionHistory.Add(CurrentPosition);
	
	if (PositionHistory.Num() > MaxHistoryEntries)
	{
		PositionHistory.RemoveAt(0);
	}
}

void UAIDebugComponent::TryBindGoapObserver()
{
	if (bGoapObserverBound)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	CachedGoapComponent = FAIComponentUtils::GetGoapComponent(Owner);
	if (!CachedGoapComponent)
	{
		return;
	}

	CachedGoapComponent->OnObservedStateChanged.AddDynamic(this, &UAIDebugComponent::HandleObservedGoapStateChanged);
	bGoapObserverBound = true;
	LastObservedState = CachedGoapComponent->GetObservedAgentState();
}

void UAIDebugComponent::HandleObservedGoapStateChanged()
{
	if (!CachedGoapComponent)
	{
		return;
	}

	const EGoapAgentState NewState = CachedGoapComponent->GetObservedAgentState();
	if (NewState != LastObservedState)
	{
		if (UWorld* World = GetWorld())
		{
			StateStartTime = World->GetTimeSeconds();
		}
		LastObservedState = NewState;
	}
}

void UAIDebugComponent::DrawDebugInfo()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
    
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
    
	FVector Location = Owner->GetActorLocation();
	
	// Draw position history trail
	if (PositionHistory.Num() > 1)
	{
		for (int32 i = 1; i < PositionHistory.Num(); ++i)
		{
			DrawDebugLine(World, PositionHistory[i - 1], PositionHistory[i],
						 DebugColor, false, 0.0f, 0, 2.0f);
		}
	}
	
	// Draw sphere at current position
	DrawDebugSphere(World, Location, 50.0f, 8, DebugColor, false, 0.0f);
	
	// Draw text information above character
	FVector TextLocation = Location + FVector(0, 0, 150.0f);
	FString InfoText = GetCurrentStateText();
	DrawDebugString(World, TextLocation, InfoText, nullptr, DebugColor, 
					0.0f, true, TextScale);
}

FString UAIDebugComponent::GetCurrentStateText() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return TEXT("No Owner");
	}
	
	TArray<FString> Lines;
	
	// Line 1: AI Name
	Lines.Add(FString::Printf(TEXT("=== %s ==="), *Owner->GetName()));
	
	// Line 2: Basic telemetry
	Lines.Add(FString::Printf(TEXT("Distance: %.1f | Actions: %d | Time: %.1fs"),
		TotalDistanceTraveled,
		ActionsCompleted,
		GetTimeInCurrentState()));
	
	// Lines 3+: GOAP information (if available and enabled)
	if (bShowGoapInfo)
	{
		if (UGoapComponent* GoapComp = CachedGoapComponent ? CachedGoapComponent : FAIComponentUtils::GetGoapComponent(Owner))
		{
			auto StateToString = [](EGoapAgentState State) -> FString
			{
				switch (State)
				{
					case EGoapAgentState::Idle: return TEXT("Idle");
					case EGoapAgentState::Planning: return TEXT("Planning");
					case EGoapAgentState::ExecutingPlan: return TEXT("Executing");
					case EGoapAgentState::PlanFailed: return TEXT("FAILED");
					default: return TEXT("Unknown");
				}
			};

			auto NameToString = [](FName Name) -> FString
			{
				return Name.IsNone() ? TEXT("None") : Name.ToString();
			};

			const EGoapAgentState LocalState = GoapComp->GetAgentState();
			const FName LocalGoalName = GoapComp->GetCurrentGoalName();
			const int32 LocalPlanLength = GoapComp->GetCurrentPlanLength();
			const int32 LocalActionIndex = GoapComp->GetCurrentActionIndex();
			const bool bLocalAsync = GoapComp->IsLocalAsyncPlanning();
			const FName LocalActionName = GoapComp->HasActiveAction()
				? FName(*GoapComp->GetCurrentAction().ActionName.ToString())
				: NAME_None;

			const EGoapAgentState ObservedState = GoapComp->GetObservedAgentState();
			const FName ObservedActionName = GoapComp->GetObservedCurrentActionName();
			const FName ObservedGoalName = GoapComp->GetObservedCurrentGoalName();
			const int32 ObservedActionIndex = GoapComp->GetObservedCurrentActionIndex();
			const int32 ObservedPlanLength = GoapComp->GetObservedPlanLength();
			const bool bObservedAsync = GoapComp->IsObservedAsyncPlanning();

			auto AddCompareLine = [&Lines](const TCHAR* Label, const FString& LocalValue, const FString& AuthValue)
			{
				const TCHAR* Match = (LocalValue == AuthValue) ? TEXT("") : TEXT(" !DIFF");
				Lines.Add(FString::Printf(TEXT("%s L:%s | A:%s%s"), Label, *LocalValue, *AuthValue, Match));
			};

			Lines.Add(TEXT("GOAP Local vs Authority"));
			AddCompareLine(TEXT("State"), StateToString(LocalState), StateToString(ObservedState));
			AddCompareLine(TEXT("Goal"), NameToString(LocalGoalName), NameToString(ObservedGoalName));
			AddCompareLine(TEXT("Action"), NameToString(LocalActionName), NameToString(ObservedActionName));
			AddCompareLine(
				TEXT("Plan"),
				FString::Printf(TEXT("%d/%d"), FMath::Clamp(LocalActionIndex + 1, 0, LocalPlanLength), LocalPlanLength),
				FString::Printf(TEXT("%d/%d"), FMath::Clamp(ObservedActionIndex + 1, 0, ObservedPlanLength), ObservedPlanLength));
			AddCompareLine(TEXT("Async"), bLocalAsync ? TEXT("Yes") : TEXT("No"), bObservedAsync ? TEXT("Yes") : TEXT("No"));
		}
		else
		{
			Lines.Add(TEXT("GOAP: Not Available"));
		}
	}
	
	// Join all lines with newlines
	return FString::Join(Lines, TEXT("\n"));
}


