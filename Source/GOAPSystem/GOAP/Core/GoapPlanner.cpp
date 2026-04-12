// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoapPlanner.h"
#include "Containers/Array.h"
#include "HAL/PlatformTime.h"
#include "GOAPSystem/GOAPSystem.h"

FGoapPlanner::FGoapPlanner()
	: MaxNodesToExplore(GoapPlanningConstants::DEFAULT_MAX_NODES)
	, NodesExplored(0)
	, PlanningTime(0.0f)
{
}

FGoapPlanner::~FGoapPlanner()
{
}

bool FGoapPlanner::Plan(
	AActor* Agent,
	const FGoapWorldState& CurrentState,
	const FGoapWorldState& GoalState,
	const TArray<FGoapActionInstance>& AvailableActions,
	TArray<FGoapActionInstance>& OutPlan,
	const FGoapPlanningOptions& PlanningOptions,
	const FGoapGoalInstance* GoalDefinition
)
{	
	// Thread-safe planning execution
	FScopeLock Lock(&PlannerMutex);
	
	const double StartTime = FPlatformTime::Seconds();
	NodesExplored = 0;
	OutPlan.Empty();
	LastFailureReason.Empty();

	// Per-plan best known path cost by state. Keeps dedupe local and thread-safe.
	TMap<FGoapWorldState, float> BestKnownGCost;
	
	const bool bUseGoalDefinition = GoalDefinition && GoalDefinition->IsValid();
	auto GoalIsSatisfied = [&GoalState, GoalDefinition, bUseGoalDefinition](const FGoapWorldState& StateToTest) -> bool
	{
		return bUseGoalDefinition ? GoalDefinition->SatisfiesDesiredState(StateToTest) : StateToTest.Satisfies(GoalState);
	};
	
	auto GoalDistance = [&GoalState, GoalDefinition, bUseGoalDefinition](const FGoapWorldState& StateToTest) -> float
	{
		return bUseGoalDefinition
			? static_cast<float>(GoalDefinition->GetDesiredStateDistance(StateToTest))
			: static_cast<float>(StateToTest.GetDistanceTo(GoalState));
	};
	
	// Validate inputs
	if (AvailableActions.Num() == 0)
	{
		LastFailureReason = TEXT("No actions configured");
		PlanningTime = static_cast<float>(FPlatformTime::Seconds() - StartTime);
		return false;
	}
	
	// Check if any actions can run from current state
	TArray<FGoapActionInstance> InitialApplicableActions;
	GetApplicableActions(Agent, CurrentState, AvailableActions, InitialApplicableActions, PlanningOptions);
	
	if (InitialApplicableActions.Num() == 0)
	{
		LastFailureReason = FString::Printf(TEXT("No applicable actions from current state (0/%d available)"), 
			AvailableActions.Num());
		PlanningTime = static_cast<float>(FPlatformTime::Seconds() - StartTime);
		const FString AgentName = Agent ? Agent->GetName() : TEXT("Unknown");
		UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=None Action=None Context=Planner Reason=%s"),
			*AgentName, *LastFailureReason);
		return false;
	}
	
	// Sort initial actions by heuristic value to prioritize promising paths
	InitialApplicableActions.Sort([&GoalDistance, &CurrentState](const FGoapActionInstance& A, const FGoapActionInstance& B)
	{
		// Apply effects and calculate heuristic for each action
		FGoapWorldState StateAfterA = CurrentState;
		StateAfterA.ApplyState(A.Effects);
		float HeuristicA = GoalDistance(StateAfterA);
		
		FGoapWorldState StateAfterB = CurrentState;
		StateAfterB.ApplyState(B.Effects);
		float HeuristicB = GoalDistance(StateAfterB);
		
		// Lower heuristic = closer to goal, should come first
		return HeuristicA < HeuristicB;
	});
	
	// Use TArray with heap operations for efficient node selection - O(log n) instead of O(n)
	TArray<TSharedPtr<FGoapPlanNode>> OpenSet;
	TSet<FGoapWorldState> ClosedSet;

	// If an initial action already reaches the goal, prefer the cheapest one-step plan.
	bool bHasDirectGoalAction = false;
	FGoapActionInstance BestDirectGoalAction;
	float BestDirectGoalCost = TNumericLimits<float>::Max();
	
	// Push sorted initial applicable actions as starting nodes (most promising first)
	for (const FGoapActionInstance& Action : InitialApplicableActions)
	{
		// Apply action effects to current state
		FGoapWorldState NewState = CurrentState;
		NewState.ApplyState(Action.Effects);
		
		const float ActionCost = CalculateActionCost(Agent, CurrentState, Action, PlanningOptions);

		if (GoalIsSatisfied(NewState))
		{
			if (!bHasDirectGoalAction || ActionCost < BestDirectGoalCost - GoapPlanningConstants::FLOAT_COMPARISON_TOLERANCE)
			{
				bHasDirectGoalAction = true;
				BestDirectGoalAction = Action;
				BestDirectGoalCost = ActionCost;
			}
		}
		
		// Create node for this action
		TSharedPtr<FGoapPlanNode> ActionNode = MakeShared<FGoapPlanNode>();
		ActionNode->ActionInstance = Action;
		ActionNode->State = NewState;
		ActionNode->GCost = ActionCost;
		ActionNode->HCost = GoalDistance(NewState);
		ActionNode->Parent = nullptr; // No parent, these are starting actions

		if (const float* ExistingBest = BestKnownGCost.Find(NewState))
		{
			if (ActionNode->GCost >= (*ExistingBest - GoapPlanningConstants::FLOAT_COMPARISON_TOLERANCE))
			{
				continue;
			}
		}
		BestKnownGCost.Add(NewState, ActionNode->GCost);
		
		// Add to heap (will maintain priority based on F-cost)
		OpenSet.HeapPush(ActionNode, FGoapNodeComparator());
	}

	if (bHasDirectGoalAction)
	{
		OutPlan.Empty();
		OutPlan.Add(BestDirectGoalAction);
		PlanningTime = static_cast<float>(FPlatformTime::Seconds() - StartTime);
		const FString AgentName = Agent ? Agent->GetName() : TEXT("Unknown");
		UE_LOG(GOAPSystem, Log, TEXT("GOAP Plan | Agent=%s Context=Planner Actions=1 Nodes=%d TimeMs=%.2f Note=DirectGoalAction"),
			*AgentName, NodesExplored, PlanningTime * 1000.0f);
		return true;
	}
	
	// A* main loop with priority queue
	while (OpenSet.Num() > 0 && NodesExplored < MaxNodesToExplore)
	{		
		// Get node with lowest F-cost (O(log n) with heap)
		TSharedPtr<FGoapPlanNode> Current;
		OpenSet.HeapPop(Current, FGoapNodeComparator());

		// Skip stale/duplicate nodes that are no longer the best path to this state.
		if (ClosedSet.Contains(Current->State))
		{
			continue;
		}
		if (const float* KnownBest = BestKnownGCost.Find(Current->State))
		{
			if (Current->GCost > (*KnownBest + GoapPlanningConstants::FLOAT_COMPARISON_TOLERANCE))
			{
				continue;
			}
		}
		
		// Check if goal reached
		if (GoalIsSatisfied(Current->State))
		{
			BuildPlan(Current, OutPlan);
			PlanningTime = static_cast<float>(FPlatformTime::Seconds() - StartTime);
			const FString AgentName = Agent ? Agent->GetName() : TEXT("Unknown");
			UE_LOG(GOAPSystem, Log, TEXT("GOAP Plan | Agent=%s Context=Planner Actions=%d Nodes=%d TimeMs=%.2f"),
				*AgentName, OutPlan.Num(), NodesExplored, PlanningTime * 1000.0f);
			return true;
		}
		
		ClosedSet.Add(Current->State);
		NodesExplored++;
		
		// Get applicable actions from current state
		TArray<FGoapActionInstance> ApplicableActions;
		GetApplicableActions(Agent, Current->State, AvailableActions, ApplicableActions, PlanningOptions);
		
		
		// Expand applicable actions
		for (const FGoapActionInstance& Action : ApplicableActions)
		{
			if (!Action.IsValid())
			{
				continue;
			}
			
			// Calculate new state after applying action
			FGoapWorldState NewState = Current->State;
			NewState.ApplyState(Action.Effects);
			
			// Skip if already explored
			if (ClosedSet.Contains(NewState))
			{
				continue;
			}
			
			// Calculate costs
			float ActionCost = CalculateActionCost(Agent, Current->State, Action, PlanningOptions);
			float NewGCost = Current->GCost + ActionCost;
			float NewHCost = GoalDistance(NewState);
			
			// Skip if we already know an equal or better path to this state.
			if (const float* ExistingBest = BestKnownGCost.Find(NewState))
			{
				if (NewGCost >= (*ExistingBest - GoapPlanningConstants::FLOAT_COMPARISON_TOLERANCE))
				{
					continue;
				}
			}
			BestKnownGCost.Add(NewState, NewGCost);
			
			// Check if we're approaching the node limit
			if (NodesExplored + OpenSet.Num() >= MaxNodesToExplore)
			{
				LastFailureReason = FString::Printf(TEXT("Max nodes limit reached (%d)"), MaxNodesToExplore);
				const FString AgentName = Agent ? Agent->GetName() : TEXT("Unknown");
				UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=None Action=None Context=Planner Reason=%s"),
					*AgentName, *LastFailureReason);
				PlanningTime = static_cast<float>(FPlatformTime::Seconds() - StartTime);
				return false;
			}
			
			// Add new node to open set (heap will maintain ordering)
			TSharedPtr<FGoapPlanNode> NewNode = MakeShared<FGoapPlanNode>(
				Action, NewState, NewGCost, NewHCost, Current
			);
			OpenSet.HeapPush(NewNode, FGoapNodeComparator());
		}
	}
	
	// Planning failed - set appropriate error message
	if (NodesExplored >= MaxNodesToExplore)
	{
		LastFailureReason = FString::Printf(TEXT("Max nodes reached (%d) - Goal may be unreachable"), MaxNodesToExplore);
	}
	else
	{
		LastFailureReason = FString::Printf(TEXT("No valid path found after exploring %d nodes"), NodesExplored);
	}
	
	PlanningTime = static_cast<float>(FPlatformTime::Seconds() - StartTime);
	const FString AgentName = Agent ? Agent->GetName() : TEXT("Unknown");
	UE_LOG(GOAPSystem, Warning, TEXT("GOAP Failure | Agent=%s Goal=None Action=None Context=Planner Reason=%s Nodes=%d TimeMs=%.2f"),
		*AgentName, *LastFailureReason, NodesExplored, PlanningTime * 1000.0f);
	
	return false;
}

void FGoapPlanner::BuildPlan(TSharedPtr<FGoapPlanNode> GoalNode, TArray<FGoapActionInstance>& OutPlan)
{
	TArray<FGoapActionInstance> ReversePlan;
	TSharedPtr<FGoapPlanNode> CurrentNode = GoalNode;
	while (CurrentNode.IsValid() && CurrentNode->ActionInstance.IsValid())
	{
	     ReversePlan.Add(CurrentNode->ActionInstance);
	     CurrentNode = CurrentNode->Parent;
	}
	
	OutPlan.Empty();
	for (int32 i = ReversePlan.Num() - 1; i >= 0; --i)
	{
	    OutPlan.Add(ReversePlan[i]);
	}
}

float FGoapPlanner::CalculateHeuristic(const FGoapWorldState& State, const FGoapWorldState& GoalState)
{
	//Implements Hemming distance
	return static_cast<float>(State.GetDistanceTo(GoalState));
}

float FGoapPlanner::CalculateActionCost(
	AActor* Agent,
	const FGoapWorldState& CurrentState,
	const FGoapActionInstance& Action,
	const FGoapPlanningOptions& PlanningOptions)
{
	float ActionCost = Action.GetCost();

	if (PlanningOptions.bEvaluateDynamicCosts && Action.SourceDataAsset)
	{
		ActionCost = Action.SourceDataAsset->GetDynamicCost(Agent, CurrentState);
	}

	if (!FMath::IsFinite(ActionCost))
	{
		return Action.GetCost();
	}

	if (PlanningOptions.bEnableActionTagCostBias)
	{
		ActionCost += Action.TagCostBias;

		if (PlanningOptions.PrioritizedActionTags.Num() > 0 && Action.ActionTags.HasAny(PlanningOptions.PrioritizedActionTags))
		{
			ActionCost -= FMath::Max(0.0f, PlanningOptions.PrioritizedTagCostDelta);
		}

		if (PlanningOptions.DeprioritizedActionTags.Num() > 0 && Action.ActionTags.HasAny(PlanningOptions.DeprioritizedActionTags))
		{
			ActionCost += FMath::Max(0.0f, PlanningOptions.DeprioritizedTagCostDelta);
		}
	}

	ActionCost = FMath::Max(0.01f, ActionCost);

	return ActionCost;
}

void FGoapPlanner::GetApplicableActions(
	AActor* Agent,
	const FGoapWorldState& CurrentState,
	const TArray<FGoapActionInstance>& AvailableActions,
	TArray<FGoapActionInstance>& OutApplicableActions,
	const FGoapPlanningOptions& PlanningOptions
)
{
	OutApplicableActions.Empty();

	const bool bCheckProcedural = PlanningOptions.bEvaluateProceduralPreconditions;
	
	for (const FGoapActionInstance& Action : AvailableActions)
	{
	    if (!Action.IsValid())
	    {
	        continue;
	    }

		if (PlanningOptions.bEnableActionTagBlocking &&
			!Action.bIgnoreComponentTagBlocks &&
			PlanningOptions.BlockedActionTags.Num() > 0 &&
			Action.ActionTags.HasAny(PlanningOptions.BlockedActionTags))
		{
			continue;
		}
	
	    // Check if preconditions are satisfied
	    if (!CurrentState.Satisfies(Action.Preconditions))
	    {
	        continue;
	    }
	
	    if (bCheckProcedural)
	    {
	    	// Check procedural preconditions (delegates to DataAsset)
	    	if (!Action.CheckProceduralPrecondition(Agent, CurrentState))
	    	{
	    		continue;
	    	}
	    }
	
	    OutApplicableActions.Add(Action);
	}
}
