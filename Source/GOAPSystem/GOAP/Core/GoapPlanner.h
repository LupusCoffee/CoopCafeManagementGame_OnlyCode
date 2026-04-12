// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GoapWorldState.h"
#include "GOAPSystem/GOAP/DataAssets/Structs/DataAssetStructs.h"
#include "GameplayTagContainer.h"

// Planning constants
namespace GoapPlanningConstants
{
	constexpr int32 DEFAULT_MAX_NODES = 1000;
	constexpr float FLOAT_COMPARISON_TOLERANCE = 0.01f;
	constexpr float DEFAULT_ACTION_COST = 1.0f;
	constexpr float DEFAULT_PLANNING_INTERVAL = 0.5f;
	constexpr float DEFAULT_ACCEPTANCE_RADIUS = 100.0f;
	constexpr float DEFAULT_WAIT_DURATION = 1.0f;
}

/** A* node for GOAP planning */
struct GOAPSYSTEM_API FGoapPlanNode
{
	FGoapActionInstance ActionInstance;
	FGoapWorldState State;
	float GCost;
	float HCost;
	TSharedPtr<FGoapPlanNode> Parent;

	FGoapPlanNode() : GCost(0.0f), HCost(0.0f), Parent(nullptr) {}

	FGoapPlanNode(const FGoapActionInstance& InActionInstance, const FGoapWorldState& InState, 
	              float InGCost, float InHCost, TSharedPtr<FGoapPlanNode> InParent)
		: ActionInstance(InActionInstance), State(InState), GCost(InGCost), HCost(InHCost), Parent(InParent) {}

	float GetFCost() const { return GCost + HCost; }
	bool operator==(const FGoapPlanNode& Other) const { return State == Other.State; }
};

/** Planning behavior toggles. Use no-UObject mode for background-thread planning. */
struct GOAPSYSTEM_API FGoapPlanningOptions
{
	/** When false, procedural preconditions are skipped to avoid UObject/Blueprint calls off the game thread. */
	bool bEvaluateProceduralPreconditions = true;

	/** When false, planner uses cached RuntimeCost and skips dynamic UObject/Blueprint cost evaluation. */
	bool bEvaluateDynamicCosts = true;

	/** Enables hard filtering of actions that contain any blocked tag. */
	bool bEnableActionTagBlocking = false;

	/** Enables additive tag-based cost adjustments. */
	bool bEnableActionTagCostBias = false;

	/** Action tags that should be filtered out from applicability. */
	FGameplayTagContainer BlockedActionTags;

	/** Action tags that should receive a cost bonus (lower cost). */
	FGameplayTagContainer PrioritizedActionTags;

	/** Action tags that should receive a cost penalty (higher cost). */
	FGameplayTagContainer DeprioritizedActionTags;

	/** Additive cost delta applied when prioritized tags match. */
	float PrioritizedTagCostDelta = 0.25f;

	/** Additive cost delta applied when deprioritized tags match. */
	float DeprioritizedTagCostDelta = 0.5f;
};

FORCEINLINE uint32 GetTypeHash(const FGoapPlanNode& Node)
{
	return GetTypeHash(Node.State);
}

/** Comparator for priority queue (min-heap based on F-cost) */
struct FGoapNodeComparator
{
	bool operator()(const TSharedPtr<FGoapPlanNode>& A, const TSharedPtr<FGoapPlanNode>& B) const
	{
		return A->GetFCost() > B->GetFCost(); // Min-heap: lower F-cost has higher priority
	}
};

/** GOAP A* planner with thread-safe operation support */
class GOAPSYSTEM_API FGoapPlanner
{
public:
	FGoapPlanner();
	~FGoapPlanner();

	/** 
	 * Plan a sequence of actions to reach the goal state.
	 * Thread-safe: Can be called from multiple threads with different instances.
	 */
	bool Plan(AActor* Agent, const FGoapWorldState& CurrentState, const FGoapWorldState& GoalState,
	          const TArray<FGoapActionInstance>& AvailableActions, TArray<FGoapActionInstance>& OutPlan,
	          const FGoapPlanningOptions& PlanningOptions = FGoapPlanningOptions(),
	          const FGoapGoalInstance* GoalDefinition = nullptr);

	void SetMaxNodesToExplore(int32 MaxNodes) { MaxNodesToExplore = MaxNodes; }
	
	/** Thread-safe getters */
	int32 GetNodesExplored() const 
	{ 
		FScopeLock Lock(&PlannerMutex);
		return NodesExplored; 
	}
	
	float GetPlanningTime() const 
	{ 
		FScopeLock Lock(&PlannerMutex);
		return PlanningTime; 
	}
	
	FString GetLastFailureReason() const 
	{ 
		FScopeLock Lock(&PlannerMutex);
		return LastFailureReason; 
	}

private:
	static void BuildPlan(TSharedPtr<FGoapPlanNode> GoalNode, TArray<FGoapActionInstance>& OutPlan);
	static float CalculateHeuristic(const FGoapWorldState& State, const FGoapWorldState& GoalState);
	static float CalculateActionCost(AActor* Agent, const FGoapWorldState& CurrentState,
	                                const FGoapActionInstance& Action,
	                                const FGoapPlanningOptions& PlanningOptions);
	static void GetApplicableActions(AActor* Agent, const FGoapWorldState& CurrentState,
	                                 const TArray<FGoapActionInstance>& AvailableActions,
	                                 TArray<FGoapActionInstance>& OutApplicableActions,
	                                 const FGoapPlanningOptions& PlanningOptions);

	int32 MaxNodesToExplore;
	int32 NodesExplored;
	float PlanningTime;
	FString LastFailureReason;
	
	/** Mutex for thread-safe access to planner state */
	mutable FCriticalSection PlannerMutex;
};
