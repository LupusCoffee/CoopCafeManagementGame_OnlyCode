// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoapActionDataAsset.h"
#include "../Core/GoapPlanner.h"

UGoapActionDataAsset::UGoapActionDataAsset()
{
	Cost = GoapPlanningConstants::DEFAULT_ACTION_COST;
	InteractionType = EGoapInteractionType::None;
	Duration = GoapPlanningConstants::DEFAULT_WAIT_DURATION;
	AcceptanceRadius = GoapPlanningConstants::DEFAULT_ACCEPTANCE_RADIUS;
	ActionName = FText::FromString("Unnamed Action");
}

bool UGoapActionDataAsset::CheckProceduralPrecondition_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const
{
	// Check float precondition checks
	for (const FGoapFloatCondition& Condition : FloatPreconditionChecks)
	{
		if (!Condition.Evaluate(CurrentState))
		{
			return false;  // Precondition check failed
		}
	}

	// Check int precondition checks
	for (const FGoapIntCondition& Condition : IntPreconditionChecks)
	{
		if (!Condition.Evaluate(CurrentState))
		{
			return false;  // Precondition check failed
		}
	}

	// Check bool precondition checks
	for (const FGoapBoolCondition& Condition : BoolPreconditionChecks)
	{
		if (!Condition.Evaluate(CurrentState))
		{
			return false;  // Precondition check failed
		}
	}

	return true;
}

float UGoapActionDataAsset::GetDynamicCost_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const
{
	return Cost;
}

FGoapWorldState UGoapActionDataAsset::BuildPreconditionState() const
{
	FGoapWorldState PreconditionState;

	// Build precondition state from bool checks
	for (const FGoapBoolCondition& Condition : BoolPreconditionChecks)
	{
		if (Condition.StateKey != NAME_None)
		{
			PreconditionState.SetBoolState(Condition.StateKey, Condition.bDesiredValue);
		}
	}
	
	for (const FGoapIntCondition& Condition : IntPreconditionChecks)
	{
		if (Condition.StateKey != NAME_None && Condition.Operator == EGoapComparisonOperator::Equal)
		{
			PreconditionState.SetIntState(Condition.StateKey, Condition.Value);
		}
	}
	
	for (const FGoapFloatCondition& Condition : FloatPreconditionChecks)
	{
		if (Condition.StateKey != NAME_None && Condition.Operator == EGoapComparisonOperator::Equal)
		{
			PreconditionState.SetFloatState(Condition.StateKey, Condition.Value);
		}
	}

	return PreconditionState;
}

void UGoapActionDataAsset::BroadcastCompletionEvent(AActor* Agent, bool bSuccess)
{
	if (!bSendTagEvents || !Agent)
	{
		return;
	}

	if (bSuccess)
	{
		if (CompletionEventTags.Num() > 0)
		{
			OnActionCompleted.Broadcast(CompletionEventTags, Agent);
		}
	}
	else
	{
		if (FailureEventTags.Num() > 0)
		{
			OnActionFailed.Broadcast(FailureEventTags, Agent);
		}
	}
}

void UGoapActionDataAsset::BroadcastStartEvent(AActor* Agent)
{
	if (!bSendTagEvents || !Agent)
	{
		return;
	}

	if (StartEventTags.Num() > 0)
	{
		OnActionStarted.Broadcast(StartEventTags, Agent);
	}
}

