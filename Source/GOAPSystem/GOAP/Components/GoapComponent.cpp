// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoapComponent.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "../Core/GoapPlanner.h"
#include "../DataAssets/Structs/DataAssetStructs.h"
#include "../Subsystems/GoapWorldSubsystem.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GOAPSystem/AI/Components/ActivatableComponent.h"
#include "GOAPSystem/AI/Components/AIDebugComponent.h"
#include "GOAPSystem/GOAP/BehaviorTree/BTTask_ExecuteGoapAction.h"
#include "GOAPSystem/GOAPSystem.h"
#include "Async/Async.h"
#include "Net/UnrealNetwork.h"

UGoapComponent::UGoapComponent()
{	
	PlanningInterval = GoapPlanningConstants::DEFAULT_PLANNING_INTERVAL;
	bReplanOnFailure = true;
	MaxPlanningNodes = GoapPlanningConstants::DEFAULT_MAX_NODES;
	CurrentActionIndex = 0;
	AgentState = EGoapAgentState::Idle;
	ObservedAgentState = EGoapAgentState::Idle;
	ObservedCurrentActionName = NAME_None;
	ObservedCurrentGoalName = NAME_None;
	PlanningTimer = 0.0f;
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UGoapComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGoapComponent, ObservedAgentState);
	DOREPLIFETIME(UGoapComponent, ObservedCurrentActionName);
	DOREPLIFETIME(UGoapComponent, ObservedCurrentGoalName);
	DOREPLIFETIME(UGoapComponent, ObservedCurrentActionIndex);
	DOREPLIFETIME(UGoapComponent, ObservedPlanLength);
	DOREPLIFETIME(UGoapComponent, bObservedAsyncPlanning);
}

void UGoapComponent::RefreshObservedState()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	ObservedAgentState = AgentState;
	ObservedCurrentActionName = (CurrentAction.IsValid() && CurrentAction.SourceDataAsset)
		? FName(*CurrentAction.SourceDataAsset->ActionName.ToString())
		: NAME_None;
	ObservedCurrentGoalName = CurrentGoal.IsValid() ? FName(*CurrentGoal.GoalName.ToString()) : NAME_None;
	ObservedCurrentActionIndex = CurrentActionIndex;
	ObservedPlanLength = CurrentPlan.Num();
	bObservedAsyncPlanning = bAsyncPlanningInProgress;

	OnObservedStateChanged.Broadcast();
}

void UGoapComponent::OnRep_ObservedState()
{
	OnObservedStateChanged.Broadcast();
}

void UGoapComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		SetComponentTickEnabled(false);
		return;
	}

	Planner.SetMaxNodesToExplore(MaxPlanningNodes);
	
	// Initialize runtime instances from templates
	InitializeRuntimeInstances();
	
	// Apply default world knowledge from subsystem
	if (UWorld* World = GetWorld())
	{
		if (UGoapWorldSubsystem* GoapWorldSubsystem = World->GetSubsystem<UGoapWorldSubsystem>())
		{
			GoapWorldSubsystem->ApplyDefaultKnowledgeToAgent(GetOwner());
		}
	}
	
	// Save the initial world state for restoration on reset
	InitialWorldState = WorldState;
	RefreshObservedState();
	
	UE_LOG(LogTemp, Verbose, TEXT("GoapComponent: Initial world state saved for %s"), 
		*GetOwner()->GetName());
}

bool UGoapComponent::CanRunGoapRuntime() const
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return false;
	}

	if (!IsComponentTickEnabled())
	{
		return false;
	}

	if (const UActivatableComponent* ActivatableComp = Owner->FindComponentByClass<UActivatableComponent>())
	{
		if (!ActivatableComp->IsActiveActor())
		{
			return false;
		}
	}

	return true;
}

void UGoapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CanRunGoapRuntime())
	{
		if (AgentState != EGoapAgentState::Idle || CurrentPlan.Num() > 0 || CurrentAction.IsValid() || bAsyncPlanningInProgress)
		{
			bAsyncPlanningInProgress = false;
			CurrentAction = FGoapActionInstance();
			CurrentPlan.Empty();
			CurrentGoal = FGoapGoalInstance();
			CurrentActionIndex = 0;
			AgentState = EGoapAgentState::Idle;
			PlanningTimer = 0.0f;
			RefreshObservedState();
		}

		return;
	}
	
	// Skip planning if async planning is in progress
	if (bAsyncPlanningInProgress)
	{
		return;
	}
	
	//Replan every set amount of time (So AI can make different decisions depending on feel)
	PlanningTimer += DeltaTime;
	if (PlanningTimer >= PlanningInterval)
	{
	    PlanningTimer = 0.0f;
	    
	    // If we have no plan or plan is invalid, replan
	    if (AgentState == EGoapAgentState::Idle || 
	        AgentState == EGoapAgentState::PlanFailed ||
	        (bReplanOnFailure && !IsPlanValid()))
	    {
	        // Log world state before replanning
	        UE_LOG(LogTemp, Log, TEXT("=== REPLANNING for %s ==="), *GetOwner()->GetName());
	        UE_LOG(LogTemp, Log, TEXT("Current World State:"));
	        LogWorldState();
	        
	        CurrentGoal = SelectBestGoal();
	        
	        if (CurrentGoal.IsValid())
	        {
	        	const bool bPlanReadyToExecute = CreatePlan();

	        	if (bPlanReadyToExecute)
	            {
	                AgentState = EGoapAgentState::ExecutingPlan;
	                CurrentActionIndex = 0;
	                StartNextAction();
	            }
	        	else if (bAsyncPlanningInProgress || AgentState == EGoapAgentState::Planning)
	        	{
	        		// Async planning is pending; stay in Planning until OnAsyncPlanningComplete decides outcome.
	        		RefreshObservedState();
	        	}
	            else
	            {
	                AgentState = EGoapAgentState::PlanFailed;
	                OnPlanFailed.Broadcast();
	                RefreshObservedState();
	            }
	        }
	        else
	        {
	            // No valid goals available - all goals achieved or no goals exist
	            // Enter idle state with no actions
	            AgentState = EGoapAgentState::Idle;
	            CurrentAction = FGoapActionInstance(); // Reset to empty instance
	            CurrentPlan.Empty();
	            CurrentActionIndex = 0;
	            RefreshObservedState();
	        	
	            UE_LOG(LogTemp, Verbose, TEXT("GOAP: No valid goals available - entering idle state"));
	        }
	    }
	}
}

// ===== World State Management (Direct access via GetWorldState) =====

void UGoapComponent::InitializeRuntimeInstances()
{
	AvailableActions.Empty();
	AvailableGoals.Empty();

	for (UGoapActionDataAsset* ActionTemplate : ActionTemplates)
	{
		if (ActionTemplate)
		{
			AvailableActions.Add(FGoapActionInstance::CreateFromDataAsset(ActionTemplate));
		}
	}

	for (UGoapGoalDataAsset* GoalTemplate : GoalTemplates)
	{
		if (GoalTemplate)
		{
			AvailableGoals.Add(FGoapGoalInstance::CreateFromDataAsset(GoalTemplate));
		}
	}
}

// ===== Planning & Execution =====

void UGoapComponent::RequestReplan()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_RequestReplan();
		return;
	}

	// Cancel the currently executing plan/action so BT can't finish stale work.
	CurrentAction = FGoapActionInstance();
	CurrentPlan.Empty();
	CurrentGoal = FGoapGoalInstance();
	CurrentActionIndex = 0;
	AgentState = EGoapAgentState::Idle;
	bAsyncPlanningInProgress = false;
	PlanningTimer = PlanningInterval;

	// Deactivated/pool states should not restart brain logic, but we still clear stale runtime state above.
	if (!CanRunGoapRuntime())
	{
		RefreshObservedState();
		return;
	}

	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController()))
		{
			AIController->StopMovement();
			if (UBrainComponent* Brain = AIController->GetBrainComponent())
			{
				Brain->StopLogic(TEXT("GOAP_RequestReplan"));
				Brain->RestartLogic();
			}
		}
	}

	RefreshObservedState();
}

void UGoapComponent::SetBlockedActionTags(const FGameplayTagContainer& NewBlockedTags, bool bRequestReplan)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetBlockedActionTags(NewBlockedTags, bRequestReplan);
		return;
	}

	BlockedActionTags = NewBlockedTags;
	if ((bRequestReplan || bAutoReplanOnTagPolicyChange) && AgentState == EGoapAgentState::ExecutingPlan)
	{
		RequestReplan();
	}
}

void UGoapComponent::AddBlockedActionTag(FGameplayTag Tag, bool bRequestReplan)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_AddBlockedActionTag(Tag, bRequestReplan);
		return;
	}

	if (Tag.IsValid())
	{
		BlockedActionTags.AddTag(Tag);
	}

	if ((bRequestReplan || bAutoReplanOnTagPolicyChange) && AgentState == EGoapAgentState::ExecutingPlan)
	{
		RequestReplan();
	}
}

void UGoapComponent::RemoveBlockedActionTag(FGameplayTag Tag, bool bRequestReplan)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_RemoveBlockedActionTag(Tag, bRequestReplan);
		return;
	}

	if (Tag.IsValid())
	{
		BlockedActionTags.RemoveTag(Tag);
	}

	if ((bRequestReplan || bAutoReplanOnTagPolicyChange) && AgentState == EGoapAgentState::ExecutingPlan)
	{
		RequestReplan();
	}
}

void UGoapComponent::SetPrioritizedActionTags(const FGameplayTagContainer& NewPrioritizedTags, bool bRequestReplan)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetPrioritizedActionTags(NewPrioritizedTags, bRequestReplan);
		return;
	}

	PrioritizedActionTags = NewPrioritizedTags;
	if ((bRequestReplan || bAutoReplanOnTagPolicyChange) && AgentState == EGoapAgentState::ExecutingPlan)
	{
		RequestReplan();
	}
}

void UGoapComponent::SetDeprioritizedActionTags(const FGameplayTagContainer& NewDeprioritizedTags, bool bRequestReplan)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetDeprioritizedActionTags(NewDeprioritizedTags, bRequestReplan);
		return;
	}

	DeprioritizedActionTags = NewDeprioritizedTags;
	if ((bRequestReplan || bAutoReplanOnTagPolicyChange) && AgentState == EGoapAgentState::ExecutingPlan)
	{
		RequestReplan();
	}
}

void UGoapComponent::ClearActionTagPolicies(bool bRequestReplan)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_ClearActionTagPolicies(bRequestReplan);
		return;
	}

	BlockedActionTags.Reset();
	PrioritizedActionTags.Reset();
	DeprioritizedActionTags.Reset();

	if ((bRequestReplan || bAutoReplanOnTagPolicyChange) && AgentState == EGoapAgentState::ExecutingPlan)
	{
		RequestReplan();
	}
}

void UGoapComponent::CompleteCurrentAction()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_CompleteCurrentAction();
		return;
	}

	if (!CurrentAction.IsValid())
	    return;
	
	// Broadcast completion event if enabled (both data asset and component events)
	if (CurrentAction.SourceDataAsset)
	{
		CurrentAction.SourceDataAsset->BroadcastCompletionEvent(GetOwner(), true);
		
		// Also broadcast through the component for centralized event handling
		if (CurrentAction.SourceDataAsset->bSendTagEvents && 
			CurrentAction.SourceDataAsset->CompletionEventTags.Num() > 0)
		{
			OnAnyActionCompleted.Broadcast(CurrentAction.SourceDataAsset->CompletionEventTags, GetOwner());
		}
	}
	
	WorldState.ApplyState(CurrentAction.Effects);
	CurrentActionIndex++;
	
	// Notify debug component for telemetry tracking
	if (AActor* Owner = GetOwner())
	{
		if (UAIDebugComponent* DebugComp = Owner->FindComponentByClass<UAIDebugComponent>())
		{
			DebugComp->IncrementActionsCompleted();
		}
	}
	
	StartNextAction();
}

void UGoapComponent::FailCurrentAction()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_FailCurrentAction();
		return;
	}

	if (!CurrentAction.IsValid())
	    return;
	
	const FString AgentName = GetOwner() ? GetOwner()->GetName() : TEXT("Unknown");
	const FString GoalName = CurrentGoal.IsValid() ? CurrentGoal.GoalName.ToString() : TEXT("None");
	const FString ActionName = CurrentAction.SourceDataAsset ? CurrentAction.SourceDataAsset->ActionName.ToString() : TEXT("Unknown");
	UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=%s Action=%s Context=Execution Reason=Current action failed"),
		*AgentName, *GoalName, *ActionName);
	
	// Broadcast failure event if enabled (both data asset and component events)
	if (CurrentAction.SourceDataAsset)
	{
		CurrentAction.SourceDataAsset->BroadcastCompletionEvent(GetOwner(), false);
		
		// Also broadcast through the component for centralized event handling
		if (CurrentAction.SourceDataAsset->bSendTagEvents && 
			CurrentAction.SourceDataAsset->FailureEventTags.Num() > 0)
		{
			OnAnyActionFailed.Broadcast(CurrentAction.SourceDataAsset->FailureEventTags, GetOwner());
		}
		
		// Apply failure effects to world state before clearing action
		WorldState.ApplyState(CurrentAction.SourceDataAsset->FailureEffects);
		
		// Log world state after applying failure effects
		UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=%s Action=%s Context=Execution Reason=Applied failure effects"),
			*AgentName, *GoalName, *ActionName);
		LogWorldState();
	}
	
	CurrentAction = FGoapActionInstance();
	CurrentPlan.Empty();
	AgentState = EGoapAgentState::PlanFailed;
	RefreshObservedState();
}

void UGoapComponent::NotifyInteractionReceived()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_NotifyInteractionReceived();
		return;
	}

	FBTExecuteGoapActionMemory* Memory = GetBehaviorTreeTaskMemory();
	if (Memory)
	{
		Memory->bInteractionReceived = true;
		UE_LOG(LogTemp, Log, TEXT("GoapComponent: Interaction received for %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapComponent: NotifyInteractionReceived called but no BT task memory available"));
	}
}

FBTExecuteGoapActionMemory* UGoapComponent::GetBehaviorTreeTaskMemory() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	AAIController* AIController = nullptr;
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		AIController = Cast<AAIController>(OwnerPawn->GetController());
	}

	if (!AIController)
	{
		AIController = Cast<AAIController>(Owner->GetInstigatorController());
	}

	if (!AIController)
		return nullptr;

	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
	if (!BTComponent)
		return nullptr;

	const UBTNode* ActiveNode = BTComponent->GetActiveNode();
	if (ActiveNode && ActiveNode->IsA<UBTTask_ExecuteGoapAction>())
	{
		if (uint8* NodeMemory = BTComponent->GetNodeMemory(ActiveNode, BTComponent->GetActiveInstanceIdx()))
		{
			return reinterpret_cast<FBTExecuteGoapActionMemory*>(NodeMemory);
		}
	}

	return nullptr;
}

void UGoapComponent::ApplyEffects(const FGoapWorldState& Effects)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	WorldState.ApplyState(Effects);
	UE_LOG(LogTemp, Warning, TEXT("ApplyEffects: Applied effects to world state for %s"), *GetOwner()->GetName());
}

void UGoapComponent::ResetExecutionState()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_ResetExecutionState();
		return;
	}

	// Clear all runtime execution state
	CurrentAction = FGoapActionInstance();
	CurrentPlan.Empty();
	CurrentGoal = FGoapGoalInstance();
	CurrentActionIndex = 0;
	AgentState = EGoapAgentState::Idle;
	PlanningTimer = 0.0f;
	
	// Restore world state to initial values from BeginPlay
	WorldState = InitialWorldState;

	// If reset happens before initial snapshot is populated (pool spawn ordering),
	// reseed from subsystem defaults so spawned AI still has baseline knowledge.
	if (WorldState.IsEmpty())
	{
		if (UWorld* World = GetWorld())
		{
			if (UGoapWorldSubsystem* GoapWorldSubsystem = World->GetSubsystem<UGoapWorldSubsystem>())
			{
				GoapWorldSubsystem->ApplyDefaultKnowledgeToAgent(GetOwner());
				InitialWorldState = WorldState;
			}
		}
	}
	
	// Reinitialize runtime instances from templates
	InitializeRuntimeInstances();
	RefreshObservedState();
	
	UE_LOG(LogTemp, Log, TEXT("GoapComponent: Execution state reset for %s (world state restored to initial values)"), 
		*GetOwner()->GetName());
}

void UGoapComponent::ModifyCurrentActionWaitDuration(float DurationDelta)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_ModifyCurrentActionWaitDuration(DurationDelta);
		return;
	}

	if (!CurrentAction.IsValid() || CurrentAction.InteractionType != EGoapInteractionType::Wait)
	{
		UE_LOG(LogTemp, Warning, TEXT("ModifyCurrentActionWaitDuration: Current action is not a wait action"));
		return;
	}

	// Modify the runtime duration (not the template Duration)
	CurrentAction.RuntimeDuration = FMath::Max(0.0f, CurrentAction.RuntimeDuration + DurationDelta);

	// Update the behavior tree memory if available
	if (FBTExecuteGoapActionMemory* Memory = GetBehaviorTreeTaskMemory())
	{
		Memory->WaitTimeRemaining = FMath::Max(0.0f, Memory->WaitTimeRemaining + DurationDelta);
		UE_LOG(LogTemp, Log, TEXT("ModifyCurrentActionWaitDuration: Modified wait time by %.2f. New remaining time: %.2f (RuntimeDuration: %.2f)"), 
			DurationDelta, Memory->WaitTimeRemaining, CurrentAction.RuntimeDuration);
	}
}

void UGoapComponent::SetCurrentActionWaitDuration(float NewDuration)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetCurrentActionWaitDuration(NewDuration);
		return;
	}

	if (!CurrentAction.IsValid() || CurrentAction.InteractionType != EGoapInteractionType::Wait)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentActionWaitDuration: Current action is not a wait action"));
		return;
	}

	NewDuration = FMath::Max(0.0f, NewDuration);
	CurrentAction.RuntimeDuration = NewDuration;  // Set runtime duration instead of template Duration

	// Update the behavior tree memory if available
	if (FBTExecuteGoapActionMemory* Memory = GetBehaviorTreeTaskMemory())
	{
		Memory->WaitTimeRemaining = NewDuration;
		UE_LOG(LogTemp, Log, TEXT("SetCurrentActionWaitDuration: Set RuntimeDuration and WaitTimeRemaining to %.2f"), NewDuration);
	}
}

float UGoapComponent::GetCurrentActionRemainingWaitTime() const
{
	if (!CurrentAction.IsValid() || CurrentAction.InteractionType != EGoapInteractionType::Wait)
		return 0.0f;

	if (FBTExecuteGoapActionMemory* Memory = GetBehaviorTreeTaskMemory())
	{
		return Memory->WaitTimeRemaining;
	}

	return 0.0f;
}

float UGoapComponent::GetCurrentActionWaitDuration() const
{
	if (CurrentAction.IsValid() && CurrentAction.InteractionType == EGoapInteractionType::Wait)
	{
		return CurrentAction.RuntimeDuration;
	}
	return 0.0f;
}

FGoapGoalInstance UGoapComponent::SelectBestGoal()
{
	if (!CanRunGoapRuntime())
	{
		return FGoapGoalInstance();
	}

	FGoapGoalInstance BestGoal;
	float HighestPriority = -1.0f;
	AActor* Owner = GetOwner();
	
	for (FGoapGoalInstance& Goal : AvailableGoals)
	{
	    if (!Goal.IsValid())
	        continue;
		
	    if (!Goal.IsValidGoal(Owner, WorldState))
	        continue;
		
	    float Priority = Goal.GetPriority(Owner, WorldState);
		if (!FMath::IsFinite(Priority))
		{
			continue;
		}

		// Cache the latest evaluated priority for debugging/inspection.
		Goal.RuntimePriority = Priority;
	    
	    if (Priority > HighestPriority)
	    {
	        HighestPriority = Priority;
	        BestGoal = Goal;
	    }
	}
	UE_LOG(LogTemp, Log, TEXT("Chosen goal: %s"), *BestGoal.GoalName.ToString());;
	return BestGoal;
}

bool UGoapComponent::CreatePlan()
{
	if (!CanRunGoapRuntime())
	{
		return false;
	}

	if (!CurrentGoal.IsValid())
	{
		const FString AgentName = GetOwner() ? GetOwner()->GetName() : TEXT("Unknown");
		UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=None Action=None Context=Planning Reason=Invalid goal"), *AgentName);
	    return false;
	}
	
	// Check if we should use async planning
	if (bUseAsyncPlanning && AvailableActions.Num() >= AsyncPlanningThreshold)
	{
		// Start async planning on background thread
		StartAsyncPlanning();
		return false; // Planning is pending and not ready for execution yet
	}
	
	// Synchronous planning (original behavior)
	AActor* Owner = GetOwner();
	FGoapWorldState DesiredState = CurrentGoal.GetDesiredState(Owner, WorldState);
	TArray<FGoapActionInstance> NewPlan;
	FGoapPlanningOptions PlanningOptions;
	PlanningOptions.bEvaluateProceduralPreconditions = true;
	PlanningOptions.bEnableActionTagBlocking = bUseActionTagPolicies;
	PlanningOptions.bEnableActionTagCostBias = bUseActionTagPolicies;
	PlanningOptions.BlockedActionTags = BlockedActionTags;
	PlanningOptions.PrioritizedActionTags = PrioritizedActionTags;
	PlanningOptions.DeprioritizedActionTags = DeprioritizedActionTags;
	PlanningOptions.PrioritizedTagCostDelta = PrioritizedTagCostDelta;
	PlanningOptions.DeprioritizedTagCostDelta = DeprioritizedTagCostDelta;

	if (Planner.Plan(Owner, WorldState, DesiredState, AvailableActions, NewPlan, PlanningOptions, &CurrentGoal))
	{
	    CurrentPlan = NewPlan;

			UE_LOG(LogTemp, Log, TEXT("GOAP: Plan created with %d actions"), CurrentPlan.Num());   for (FGoapActionInstance Plan : NewPlan)
	    {
				UE_LOG(LogTemp, Log, TEXT("Action: %s"), *Plan.SourceDataAsset->ActionName.ToString());
	    }
		
	    return true;
	}
	
	// Planning failed - log concise error message
	FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");
	FString GoalName = CurrentGoal.GoalName.ToString();
	FString FailureReason = Planner.GetLastFailureReason();
	
	UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=%s Action=None Context=Planning Reason=%s"),
		*OwnerName, *GoalName, *FailureReason);
	
	return false;
}

void UGoapComponent::StartNextAction()
{
	if (!CanRunGoapRuntime())
	{
		CurrentAction = FGoapActionInstance();
		CurrentPlan.Empty();
		CurrentGoal = FGoapGoalInstance();
		CurrentActionIndex = 0;
		AgentState = EGoapAgentState::Idle;
		RefreshObservedState();
		return;
	}

	if (CurrentActionIndex >= 0 && CurrentActionIndex < CurrentPlan.Num())
	{
	    CurrentAction = CurrentPlan[CurrentActionIndex];
	    
	    if (CurrentAction.IsValid())
	    {
	        OnActionStarted.Broadcast(CurrentAction.SourceDataAsset);

			if (CurrentAction.SourceDataAsset)
			{
				CurrentAction.SourceDataAsset->BroadcastStartEvent(GetOwner());
				if (CurrentAction.SourceDataAsset->bSendTagEvents && CurrentAction.SourceDataAsset->StartEventTags.Num() > 0)
				{
					OnAnyActionStartedWithTags.Broadcast(CurrentAction.SourceDataAsset->StartEventTags, GetOwner(), CurrentAction.SourceDataAsset);
				}
			}
	    }
	}
	else
	{
	    CurrentAction = FGoapActionInstance();
		if (!bAsyncPlanningInProgress && AgentState != EGoapAgentState::Planning)
		{
	    		AgentState = EGoapAgentState::Idle;
		}
	}

	RefreshObservedState();
}

bool UGoapComponent::IsPlanValid() const
{
	if (CurrentPlan.Num() == 0 || CurrentActionIndex < 0)
		return false;
	
	if (CurrentActionIndex < CurrentPlan.Num())
	{
		const FGoapActionInstance& NextAction = CurrentPlan[CurrentActionIndex];
		return NextAction.IsValid() && WorldState.Satisfies(NextAction.Preconditions);
	}

	return true;
}

// ===== Runtime Actor Assignment Helpers =====

void UGoapComponent::AssignTargetActor(FName Key, AActor* TargetActor)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_AssignTargetActor(Key, TargetActor);
		return;
	}

	if (Key.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("GoapComponent: Cannot assign target actor with empty key"));
		return;
	}

	WorldState.SetActorState(Key, TargetActor);
	
	// Optionally trigger a replan if needed
	// RequestReplan();
}

void UGoapComponent::RemoveTargetActor(FName Key)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_RemoveTargetActor(Key);
		return;
	}

	WorldState.RemoveActorState(Key);
	
	// Optionally trigger a replan if needed
	// RequestReplan();
}

void UGoapComponent::SetBoolState(FName Key, bool Value)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetBoolState(Key, Value);
		return;
	}

	WorldState.SetBoolState(Key, Value);
}

void UGoapComponent::SetIntState(FName Key, int32 Value)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetIntState(Key, Value);
		return;
	}

	WorldState.SetIntState(Key, Value);
}

void UGoapComponent::SetFloatState(FName Key, float Value)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetFloatState(Key, Value);
		return;
	}

	WorldState.SetFloatState(Key, Value);
}

void UGoapComponent::SetObjectState(FName Key, UObject* Value)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_SetObjectState(Key, Value);
		return;
	}

	WorldState.SetObjectState(Key, Value);
}

void UGoapComponent::RefreshDefaultKnowledge()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_RefreshDefaultKnowledge();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UGoapWorldSubsystem* GoapWorldSubsystem = World->GetSubsystem<UGoapWorldSubsystem>())
		{
			GoapWorldSubsystem->ApplyDefaultKnowledgeToAgent(GetOwner());
		}
	}
}

void UGoapComponent::LogWorldState() const
{
	// Log Bool States
	if (WorldState.GetBoolStates().Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("  Bool States:"));
		for (const auto& Pair : WorldState.GetBoolStates())
		{
			UE_LOG(LogTemp, Log, TEXT("    %s = %s"), *Pair.Key.ToString(), Pair.Value ? TEXT("true") : TEXT("false"));
		}
	}
	
	// Log Int States
	if (WorldState.GetIntStates().Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("  Int States:"));
		for (const auto& Pair : WorldState.GetIntStates())
		{
			UE_LOG(LogTemp, Log, TEXT("    %s = %d"), *Pair.Key.ToString(), Pair.Value);
		}
	}
	
	// Log Float States
	if (WorldState.GetFloatStates().Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("  Float States:"));
		for (const auto& Pair : WorldState.GetFloatStates())
		{
			UE_LOG(LogTemp, Log, TEXT("    %s = %.2f"), *Pair.Key.ToString(), Pair.Value);
		}
	}
	
	// Log Object States
	if (WorldState.GetObjectStates().Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("  Object/Actor States:"));
		for (const auto& Pair : WorldState.GetObjectStates())
		{
			if (Pair.Value)
			{
				if (AActor* Actor = Cast<AActor>(Pair.Value))
				{
					UE_LOG(LogTemp, Log, TEXT("    %s = %s (Actor)"), *Pair.Key.ToString(), *Actor->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("    %s = %s (Object)"), *Pair.Key.ToString(), *Pair.Value->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("    %s = nullptr"), *Pair.Key.ToString());
			}
		}
	}
	
	if (WorldState.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("  (World state is empty)"));
	}
}

// ===== Async Planning Implementation =====

void UGoapComponent::StartAsyncPlanning()
{
	if (!CanRunGoapRuntime())
	{
		return;
	}

	if (bAsyncPlanningInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("GOAP [%s]: Async planning already in progress"), *GetOwner()->GetName());
		return;
	}

	// Mark planning as in progress
	bAsyncPlanningInProgress = true;
	AgentState = EGoapAgentState::Planning;
	RefreshObservedState();
	
	// Snapshot current state for planning
	AsyncPlanningWorldStateSnapshot = WorldState;
	AsyncPlanningGoal = CurrentGoal;
	
	// Capture data for background thread (copies to avoid threading issues)
	AActor* Owner = GetOwner();
	FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");
	FGoapWorldState CurrentWorldState = WorldState;
	FGoapWorldState DesiredState = CurrentGoal.GetDesiredState(Owner, WorldState);
	TArray<FGoapActionInstance> ActionsCopy;
	ActionsCopy.Reserve(AvailableActions.Num());

	// Snapshot procedural preconditions on the game thread so worker planning stays UObject/Blueprint-free.
	for (const FGoapActionInstance& Action : AvailableActions)
	{
		if (!Action.IsValid())
		{
			continue;
		}

		if (!Action.CheckProceduralPrecondition(Owner, CurrentWorldState))
		{
			continue;
		}

		ActionsCopy.Add(Action);
	}
	int32 MaxNodes = MaxPlanningNodes;
	const bool bUseTagPoliciesForPlan = bUseActionTagPolicies;
	const FGameplayTagContainer BlockedTagsForPlan = BlockedActionTags;
	const FGameplayTagContainer PrioritizedTagsForPlan = PrioritizedActionTags;
	const FGameplayTagContainer DeprioritizedTagsForPlan = DeprioritizedActionTags;
	const float PrioritizedDeltaForPlan = PrioritizedTagCostDelta;
	const float DeprioritizedDeltaForPlan = DeprioritizedTagCostDelta;
	
	// Weak pointer to this component for callback
	TWeakObjectPtr<UGoapComponent> WeakThis(this);
	
	UE_LOG(LogTemp, Log, TEXT("GOAP [%s]: Starting async planning (%d actions, %d max nodes)"), 
		*OwnerName, ActionsCopy.Num(), MaxNodes);
	
	// Start async task on background thread
	const FGoapGoalInstance GoalForPlanning = CurrentGoal;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, CurrentWorldState, DesiredState, ActionsCopy, MaxNodes, OwnerName, GoalForPlanning, bUseTagPoliciesForPlan, BlockedTagsForPlan, PrioritizedTagsForPlan, DeprioritizedTagsForPlan, PrioritizedDeltaForPlan, DeprioritizedDeltaForPlan]()
	{
		// Create temporary planner for this thread
		FGoapPlanner BackgroundPlanner;
		BackgroundPlanner.SetMaxNodesToExplore(MaxNodes);
		FGoapPlanningOptions PlanningOptions;
		PlanningOptions.bEvaluateProceduralPreconditions = false;
		PlanningOptions.bEvaluateDynamicCosts = false;
		PlanningOptions.bEnableActionTagBlocking = bUseTagPoliciesForPlan;
		PlanningOptions.bEnableActionTagCostBias = bUseTagPoliciesForPlan;
		PlanningOptions.BlockedActionTags = BlockedTagsForPlan;
		PlanningOptions.PrioritizedActionTags = PrioritizedTagsForPlan;
		PlanningOptions.DeprioritizedActionTags = DeprioritizedTagsForPlan;
		PlanningOptions.PrioritizedTagCostDelta = PrioritizedDeltaForPlan;
		PlanningOptions.DeprioritizedTagCostDelta = DeprioritizedDeltaForPlan;
		
		TArray<FGoapActionInstance> BackgroundPlan;
		bool bPlanSuccess = BackgroundPlanner.Plan(nullptr, CurrentWorldState, DesiredState, ActionsCopy, BackgroundPlan, PlanningOptions, &GoalForPlanning);
		
		// Log planning results
		if (bPlanSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("GOAP [%s]: Async planning succeeded with %d actions (%.2fms)"), 
				*OwnerName, BackgroundPlan.Num(), BackgroundPlanner.GetPlanningTime() * 1000.0f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GOAP [%s]: Async planning failed - %s"), 
				*OwnerName, *BackgroundPlanner.GetLastFailureReason());
		}
		
		// Return to game thread to apply results
		AsyncTask(ENamedThreads::GameThread, [WeakThis, bPlanSuccess, BackgroundPlan]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->OnAsyncPlanningComplete(bPlanSuccess, BackgroundPlan);
			}
		});
	});
}

void UGoapComponent::OnAsyncPlanningComplete(bool bSuccess, const TArray<FGoapActionInstance>& NewPlan)
{
	if (!CanRunGoapRuntime())
	{
		bAsyncPlanningInProgress = false;
		return;
	}

	// Clear async planning flag
	bAsyncPlanningInProgress = false;
	
	AActor* Owner = GetOwner();
	FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");
	
	// Validate that world state hasn't changed significantly during planning
	// If goal or state changed, the plan might be invalid
	if (!CurrentGoal.IsValid() || !CurrentGoal.GoalName.EqualTo(AsyncPlanningGoal.GoalName))
	{
		UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=%s Action=None Context=AsyncPlanning Reason=Goal changed during planning"),
			*OwnerName, *CurrentGoal.GoalName.ToString());
		AgentState = EGoapAgentState::Idle;
		RefreshObservedState();
		return;
	}

	if (WorldState != AsyncPlanningWorldStateSnapshot)
	{
		UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=%s Action=None Context=AsyncPlanning Reason=World state changed during planning"),
			*OwnerName, *CurrentGoal.GoalName.ToString());
		AgentState = EGoapAgentState::Idle;
		RefreshObservedState();
		return;
	}
	
	if (bSuccess && NewPlan.Num() > 0)
	{
		// Apply the new plan
		CurrentPlan = NewPlan;
		AgentState = EGoapAgentState::ExecutingPlan;
		CurrentActionIndex = 0;
		StartNextAction();
		
		UE_LOG(LogTemp, Log, TEXT("GOAP [%s]: Applied async plan with %d actions"), *OwnerName, NewPlan.Num());
	}
	else
	{
		// Planning failed
		AgentState = EGoapAgentState::PlanFailed;
		OnPlanFailed.Broadcast();
		RefreshObservedState();
		
		UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=%s Action=None Context=AsyncPlanning Reason=No valid plan produced"),
			*OwnerName, *CurrentGoal.GoalName.ToString());
	}
}

void UGoapComponent::Server_RequestReplan_Implementation()
{
	RequestReplan();
}

void UGoapComponent::Server_SetBlockedActionTags_Implementation(const FGameplayTagContainer& NewBlockedTags, bool bRequestReplan)
{
	SetBlockedActionTags(NewBlockedTags, bRequestReplan);
}

void UGoapComponent::Server_AddBlockedActionTag_Implementation(FGameplayTag Tag, bool bRequestReplan)
{
	AddBlockedActionTag(Tag, bRequestReplan);
}

void UGoapComponent::Server_RemoveBlockedActionTag_Implementation(FGameplayTag Tag, bool bRequestReplan)
{
	RemoveBlockedActionTag(Tag, bRequestReplan);
}

void UGoapComponent::Server_SetPrioritizedActionTags_Implementation(const FGameplayTagContainer& NewPrioritizedTags, bool bRequestReplan)
{
	SetPrioritizedActionTags(NewPrioritizedTags, bRequestReplan);
}

void UGoapComponent::Server_SetDeprioritizedActionTags_Implementation(const FGameplayTagContainer& NewDeprioritizedTags, bool bRequestReplan)
{
	SetDeprioritizedActionTags(NewDeprioritizedTags, bRequestReplan);
}

void UGoapComponent::Server_ClearActionTagPolicies_Implementation(bool bRequestReplan)
{
	ClearActionTagPolicies(bRequestReplan);
}

void UGoapComponent::Server_CompleteCurrentAction_Implementation()
{
	CompleteCurrentAction();
}

void UGoapComponent::Server_FailCurrentAction_Implementation()
{
	FailCurrentAction();
}

void UGoapComponent::Server_NotifyInteractionReceived_Implementation()
{
	NotifyInteractionReceived();
}

void UGoapComponent::Server_ResetExecutionState_Implementation()
{
	ResetExecutionState();
}

void UGoapComponent::Server_ModifyCurrentActionWaitDuration_Implementation(float DurationDelta)
{
	ModifyCurrentActionWaitDuration(DurationDelta);
}

void UGoapComponent::Server_SetCurrentActionWaitDuration_Implementation(float NewDuration)
{
	SetCurrentActionWaitDuration(NewDuration);
}

void UGoapComponent::Server_AssignTargetActor_Implementation(FName Key, AActor* TargetActor)
{
	AssignTargetActor(Key, TargetActor);
}

void UGoapComponent::Server_RemoveTargetActor_Implementation(FName Key)
{
	RemoveTargetActor(Key);
}

void UGoapComponent::Server_RefreshDefaultKnowledge_Implementation()
{
	RefreshDefaultKnowledge();
}

void UGoapComponent::Server_SetBoolState_Implementation(FName Key, bool Value)
{
	SetBoolState(Key, Value);
}

void UGoapComponent::Server_SetIntState_Implementation(FName Key, int32 Value)
{
	SetIntState(Key, Value);
}

void UGoapComponent::Server_SetFloatState_Implementation(FName Key, float Value)
{
	SetFloatState(Key, Value);
}

void UGoapComponent::Server_SetObjectState_Implementation(FName Key, UObject* Value)
{
	SetObjectState(Key, Value);
}





