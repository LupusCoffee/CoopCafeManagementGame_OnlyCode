// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GoapWorldState.h"
#include "../BehaviorTree/BTTask_ExecuteGoapActionMemory.h"
#include "../DataAssets/GoapActionDataAsset.h"
#include "../DataAssets/GoapGoalDataAsset.h"
#include "../Core/GoapPlanner.h"
#include "GoapComponent.generated.h"

/**
 * Enum for agent state
 */
UENUM(BlueprintType)
enum class EGoapAgentState : uint8
{
	Idle,
	Planning,
	ExecutingPlan,
	PlanFailed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionTimeoutSignature, 
	UGoapActionDataAsset*, FailedAction, 
	const FGoapWorldState&, UpdatedWorldState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActionTagHookSignature,
	const FGameplayTagContainer&, HookTags,
	AActor*, Agent,
	UGoapActionDataAsset*, Action);

/**
 * GOAP planning and execution component.
 * BT reads CurrentAction and calls CompleteCurrentAction() when done.
 */
UCLASS(ClassGroup=(GOAPSystem), meta=(BlueprintSpawnableComponent))
class GOAPSYSTEM_API UGoapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGoapComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ===== Configuration =====

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
	TArray<TObjectPtr<UGoapActionDataAsset>> ActionTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
	TArray<TObjectPtr<UGoapGoalDataAsset>> GoalTemplates;
	
	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	TArray<FGoapActionInstance> AvailableActions;

	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	TArray<FGoapGoalInstance> AvailableGoals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	float PlanningInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	bool bReplanOnFailure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	int32 MaxPlanningNodes;

	/** Enables component-level tag filters and planner cost bias using each action's ActionTags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Action Tags")
	bool bUseActionTagPolicies = true;

	/** If enabled, changing tag policy containers can immediately trigger RequestReplan(). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Action Tags")
	bool bAutoReplanOnTagPolicyChange = false;

	/** Any action with matching tags is blocked during planning unless action opts out. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP|Action Tags")
	FGameplayTagContainer BlockedActionTags;

	/** Any action with matching tags gets a lower planning cost. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP|Action Tags")
	FGameplayTagContainer PrioritizedActionTags;

	/** Any action with matching tags gets a higher planning cost. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP|Action Tags")
	FGameplayTagContainer DeprioritizedActionTags;

	/** Cost reduction applied when an action matches PrioritizedActionTags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Action Tags", meta = (ClampMin = "0.0"))
	float PrioritizedTagCostDelta = 0.25f;

	/** Cost increase applied when an action matches DeprioritizedActionTags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Action Tags", meta = (ClampMin = "0.0"))
	float DeprioritizedTagCostDelta = 0.5f;

	/** Enable async planning (plans on background thread to avoid blocking game thread) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Async")
	bool bUseAsyncPlanning = false;

	/** Minimum action count to trigger async planning (below this, plans synchronously for speed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Async", meta = (EditCondition = "bUseAsyncPlanning"))
	int32 AsyncPlanningThreshold = 20;

	// ===== Core Functions =====

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void RequestReplan();

	UFUNCTION(BlueprintCallable, Category = "GOAP|Action Tags")
	void SetBlockedActionTags(const FGameplayTagContainer& NewBlockedTags, bool bRequestReplan = true);

	UFUNCTION(BlueprintCallable, Category = "GOAP|Action Tags")
	void AddBlockedActionTag(FGameplayTag Tag, bool bRequestReplan = true);

	UFUNCTION(BlueprintCallable, Category = "GOAP|Action Tags")
	void RemoveBlockedActionTag(FGameplayTag Tag, bool bRequestReplan = true);

	UFUNCTION(BlueprintCallable, Category = "GOAP|Action Tags")
	void SetPrioritizedActionTags(const FGameplayTagContainer& NewPrioritizedTags, bool bRequestReplan = false);

	UFUNCTION(BlueprintCallable, Category = "GOAP|Action Tags")
	void SetDeprioritizedActionTags(const FGameplayTagContainer& NewDeprioritizedTags, bool bRequestReplan = false);

	UFUNCTION(BlueprintCallable, Category = "GOAP|Action Tags")
	void ClearActionTagPolicies(bool bRequestReplan = true);

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void CompleteCurrentAction();

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void FailCurrentAction();
	
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void NotifyInteractionReceived();

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void ApplyEffects(const FGoapWorldState& Effects);

	/** Resets execution state (plan, actions, goals) while preserving templates and world state */
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void ResetExecutionState();

	/** Modify the wait duration of the current action (if it's a wait action). Positive values increase duration, negative decrease it. */
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void ModifyCurrentActionWaitDuration(float DurationDelta);

	/** Set the wait duration of the current action to a specific value (if it's a wait action). */
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetCurrentActionWaitDuration(float NewDuration);

	/** Get the remaining wait time of the current action (returns 0 if not a wait action or no action is running). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	float GetCurrentActionRemainingWaitTime() const;

	/** Get the total wait duration of the current action (returns 0 if not a wait action). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	float GetCurrentActionWaitDuration() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	FGoapActionInstance GetCurrentAction() const { return CurrentAction; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	bool HasActiveAction() const { return CurrentAction.IsValid(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	EGoapAgentState GetAgentState() const { return AgentState; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	FName GetCurrentGoalName() const { return CurrentGoal.IsValid() ? FName(*CurrentGoal.GoalName.ToString()) : NAME_None; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	int32 GetCurrentActionIndex() const { return CurrentActionIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	int32 GetCurrentPlanLength() const { return CurrentPlan.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Async")
	bool IsLocalAsyncPlanning() const { return bAsyncPlanningInProgress; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Net")
	EGoapAgentState GetObservedAgentState() const { return ObservedAgentState; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Net")
	FName GetObservedCurrentActionName() const { return ObservedCurrentActionName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Net")
	FName GetObservedCurrentGoalName() const { return ObservedCurrentGoalName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Net")
	int32 GetObservedCurrentActionIndex() const { return ObservedCurrentActionIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Net")
	int32 GetObservedPlanLength() const { return ObservedPlanLength; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Net")
	bool IsObservedAsyncPlanning() const { return bObservedAsyncPlanning; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP")
	const FGoapWorldState& GetWorldState() const { return WorldState; }

	/** Check if async planning is currently in progress */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GOAP|Async")
	bool IsAsyncPlanningInProgress() const { return bAsyncPlanningInProgress; }

	// ===== Runtime Actor Assignment Helpers =====
	
	/** Assign a target actor to this agent's world state for the given key */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Runtime")
	void AssignTargetActor(FName Key, AActor* TargetActor);

	/** Remove a target actor from this agent's world state */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Runtime")
	void RemoveTargetActor(FName Key);

	/** Refresh default world knowledge from the subsystem */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Runtime")
	void RefreshDefaultKnowledge();

	/** Log the current world state for debugging */
	void LogWorldState() const;

	// ===== Events =====

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlanFailed);
	UPROPERTY(BlueprintAssignable, Category = "GOAP")
	FOnPlanFailed OnPlanFailed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObservedStateChanged);
	UPROPERTY(BlueprintAssignable, Category = "GOAP|Net")
	FOnObservedStateChanged OnObservedStateChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionStarted, UGoapActionDataAsset*, Action);
	UPROPERTY(BlueprintAssignable, Category = "GOAP")
	FOnActionStarted OnActionStarted;
	
	UPROPERTY(BlueprintAssignable, Category = "GOAP|Events")
	FOnActionTimeoutSignature OnActionTimeout;

	/** 
	 * Fires when any action completes successfully (if bSendTagEvents is enabled on the action).
	 * Subscribe to this event on the GOAP Component to handle all action completions in one place.
	 */
	UPROPERTY(BlueprintAssignable, Category = "GOAP|Events")
	FOnGoapActionCompletedWithTags OnAnyActionCompleted;

	/** 
	 * Fires when any action fails (if bSendTagEvents is enabled on the action).
	 * Subscribe to this event on the GOAP Component to handle all action failures in one place.
	 */
	UPROPERTY(BlueprintAssignable, Category = "GOAP|Events")
	FOnGoapActionCompletedWithTags OnAnyActionFailed;

	/** Fires when an action starts and has tag hooks configured. */
	UPROPERTY(BlueprintAssignable, Category = "GOAP|Events")
	FOnActionTagHookSignature OnAnyActionStartedWithTags;
	
	// ===== WorldState Setters =====
	
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void SetBoolState(FName Key, bool Value);
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void SetIntState(FName Key, int32 Value);
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void SetFloatState(FName Key, float Value);
	UFUNCTION(BlueprintCallable, Category = "GOAP|World State")
	void SetObjectState(FName Key, UObject* Value);

protected:
	/** Returns true only when this component is allowed to run planning/execution logic. */
	bool CanRunGoapRuntime() const;

	void InitializeRuntimeInstances();
	FGoapGoalInstance SelectBestGoal();
	bool CreatePlan();
	void StartNextAction();
	bool IsPlanValid() const;

	/** Helper to get BehaviorTree task memory (returns nullptr if not available) */
	FBTExecuteGoapActionMemory* GetBehaviorTreeTaskMemory() const;

	// ===== Async Planning =====
	
	/** Start async planning on background thread */
	void StartAsyncPlanning();
	
	/** Called on game thread when async planning completes */
	void OnAsyncPlanningComplete(bool bSuccess, const TArray<FGoapActionInstance>& NewPlan);

	/** Mirror lightweight runtime state for clients/UI */
	void RefreshObservedState();
	
	/** Async planning state tracking */
	bool bAsyncPlanningInProgress = false;
	
	/** Snapshot of world state when async planning started (for validation) */
	FGoapWorldState AsyncPlanningWorldStateSnapshot;
	
	/** Goal being planned for asynchronously */
	FGoapGoalInstance AsyncPlanningGoal;

	FGoapPlanner Planner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	FGoapWorldState WorldState;

	// Stores the initial world state from BeginPlay for restoration on reset
	UPROPERTY()
	FGoapWorldState InitialWorldState;

	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	FGoapGoalInstance CurrentGoal;

	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	TArray<FGoapActionInstance> CurrentPlan;

	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	FGoapActionInstance CurrentAction;

	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	int32 CurrentActionIndex;

	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	EGoapAgentState AgentState;

	UPROPERTY(ReplicatedUsing = OnRep_ObservedState, BlueprintReadOnly, Category = "GOAP|Net")
	EGoapAgentState ObservedAgentState;

	UPROPERTY(ReplicatedUsing = OnRep_ObservedState, BlueprintReadOnly, Category = "GOAP|Net")
	FName ObservedCurrentActionName;

	UPROPERTY(ReplicatedUsing = OnRep_ObservedState, BlueprintReadOnly, Category = "GOAP|Net")
	FName ObservedCurrentGoalName;

	UPROPERTY(ReplicatedUsing = OnRep_ObservedState, BlueprintReadOnly, Category = "GOAP|Net")
	int32 ObservedCurrentActionIndex = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ObservedState, BlueprintReadOnly, Category = "GOAP|Net")
	int32 ObservedPlanLength = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ObservedState, BlueprintReadOnly, Category = "GOAP|Net")
	bool bObservedAsyncPlanning = false;

	UFUNCTION()
	void OnRep_ObservedState();

	float PlanningTimer;

private:
	UFUNCTION(Server, Reliable)
	void Server_RequestReplan();

	UFUNCTION(Server, Reliable)
	void Server_SetBlockedActionTags(const FGameplayTagContainer& NewBlockedTags, bool bRequestReplan);

	UFUNCTION(Server, Reliable)
	void Server_AddBlockedActionTag(FGameplayTag Tag, bool bRequestReplan);

	UFUNCTION(Server, Reliable)
	void Server_RemoveBlockedActionTag(FGameplayTag Tag, bool bRequestReplan);

	UFUNCTION(Server, Reliable)
	void Server_SetPrioritizedActionTags(const FGameplayTagContainer& NewPrioritizedTags, bool bRequestReplan);

	UFUNCTION(Server, Reliable)
	void Server_SetDeprioritizedActionTags(const FGameplayTagContainer& NewDeprioritizedTags, bool bRequestReplan);

	UFUNCTION(Server, Reliable)
	void Server_ClearActionTagPolicies(bool bRequestReplan);

	UFUNCTION(Server, Reliable)
	void Server_CompleteCurrentAction();

	UFUNCTION(Server, Reliable)
	void Server_FailCurrentAction();

	UFUNCTION(Server, Reliable)
	void Server_NotifyInteractionReceived();

	UFUNCTION(Server, Reliable)
	void Server_ResetExecutionState();

	UFUNCTION(Server, Reliable)
	void Server_ModifyCurrentActionWaitDuration(float DurationDelta);

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentActionWaitDuration(float NewDuration);

	UFUNCTION(Server, Reliable)
	void Server_AssignTargetActor(FName Key, AActor* TargetActor);

	UFUNCTION(Server, Reliable)
	void Server_RemoveTargetActor(FName Key);

	UFUNCTION(Server, Reliable)
	void Server_RefreshDefaultKnowledge();

	UFUNCTION(Server, Reliable)
	void Server_SetBoolState(FName Key, bool Value);

	UFUNCTION(Server, Reliable)
	void Server_SetIntState(FName Key, int32 Value);

	UFUNCTION(Server, Reliable)
	void Server_SetFloatState(FName Key, float Value);

	UFUNCTION(Server, Reliable)
	void Server_SetObjectState(FName Key, UObject* Value);
};
