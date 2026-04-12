// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoapGoalDataAsset.h"

// ===== FGoapFloatCondition =====

bool FGoapFloatCondition::Evaluate(const FGoapWorldState& WorldState) const
{
	if (StateKey == NAME_None)
	{
		return true;  // Invalid condition, assume true
	}

	if (!WorldState.HasFloatState(StateKey))
	{
		return false;  // State key not found
	}

	float CurrentValue = WorldState.GetFloatState(StateKey);

	switch (Operator)
	{
		case EGoapComparisonOperator::Equal:
			return FMath::IsNearlyEqual(CurrentValue, Value, 0.001f);
		case EGoapComparisonOperator::NotEqual:
			return !FMath::IsNearlyEqual(CurrentValue, Value, 0.001f);
		case EGoapComparisonOperator::GreaterThan:
			return CurrentValue > Value;
		case EGoapComparisonOperator::GreaterOrEqual:
			return CurrentValue >= Value;
		case EGoapComparisonOperator::LessThan:
			return CurrentValue < Value;
		case EGoapComparisonOperator::LessOrEqual:
			return CurrentValue <= Value;
		default:
			return false;
	}
}

// ===== FGoapIntCondition =====

bool FGoapIntCondition::Evaluate(const FGoapWorldState& WorldState) const
{
	if (StateKey == NAME_None)
	{
		return true;  // Invalid condition, assume true
	}

	if (!WorldState.HasIntState(StateKey))
	{
		return false;  // State key not found
	}

	int32 CurrentValue = WorldState.GetIntState(StateKey);

	switch (Operator)
	{
		case EGoapComparisonOperator::Equal:
			return CurrentValue == Value;
		case EGoapComparisonOperator::NotEqual:
			return CurrentValue != Value;
		case EGoapComparisonOperator::GreaterThan:
			return CurrentValue > Value;
		case EGoapComparisonOperator::GreaterOrEqual:
			return CurrentValue >= Value;
		case EGoapComparisonOperator::LessThan:
			return CurrentValue < Value;
		case EGoapComparisonOperator::LessOrEqual:
			return CurrentValue <= Value;
		default:
			return false;
	}
}

// ===== FGoapBoolCondition =====

bool FGoapBoolCondition::Evaluate(const FGoapWorldState& WorldState) const
{
	if (StateKey == NAME_None)
	{
		return true;  // Invalid condition, assume true
	}

	if (!WorldState.HasBoolState(StateKey))
	{
		return false;  // State key not found
	}

	bool CurrentValue = WorldState.GetBoolState(StateKey);
	return CurrentValue == bDesiredValue;
}

// ===== UGoapGoalDataAsset =====

UGoapGoalDataAsset::UGoapGoalDataAsset()
{
	BasePriority = 1.0f;
	GoalName = FText::FromString("Unnamed Goal");
	Description = FText::FromString("Base GOAP Goal Data Asset");
}

float UGoapGoalDataAsset::GetPriority_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const
{
	return BasePriority;
}

bool UGoapGoalDataAsset::IsValid_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const
{
	// Check float validation conditions
	for (const FGoapFloatCondition& Condition : FloatValidationConditions)
	{
		if (!Condition.Evaluate(CurrentState))
		{
			return false;  // Validation condition not met
		}
	}

	// Check int validation conditions
	for (const FGoapIntCondition& Condition : IntValidationConditions)
	{
		if (!Condition.Evaluate(CurrentState))
		{
			return false;  // Validation condition not met
		}
	}

	// Check bool validation conditions
	for (const FGoapBoolCondition& Condition : BoolValidationConditions)
	{
		if (!Condition.Evaluate(CurrentState))
		{
			return false;  // Validation condition not met
		}
	}
	
	// IMPORTANT: Goal is invalid if already achieved!
	// This allows the system to automatically switch to lower priority goals
	if (DesiredBoolStates.Num() > 0 || DesiredObjectStates.Num() > 0 || IntDesiredConditions.Num() > 0 || FloatDesiredConditions.Num() > 0)
	{
		bool bDesiredSatisfied = true;

		for (const auto& Pair : DesiredBoolStates)
		{
			if (CurrentState.GetBoolState(Pair.Key, !Pair.Value) != Pair.Value)
			{
				bDesiredSatisfied = false;
				break;
			}
		}

		if (bDesiredSatisfied)
		{
			for (const auto& Pair : DesiredObjectStates)
			{
				if (CurrentState.GetObjectState(Pair.Key) != Pair.Value)
				{
					bDesiredSatisfied = false;
					break;
				}
			}
		}

		if (bDesiredSatisfied)
		{
			for (const FGoapIntCondition& Condition : IntDesiredConditions)
			{
				if (!Condition.Evaluate(CurrentState))
				{
					bDesiredSatisfied = false;
					break;
				}
			}
		}

		if (bDesiredSatisfied)
		{
			for (const FGoapFloatCondition& Condition : FloatDesiredConditions)
			{
				if (!Condition.Evaluate(CurrentState))
				{
					bDesiredSatisfied = false;
					break;
				}
			}
		}

		if (bDesiredSatisfied)
		{
			return false;  // Goal already satisfied - no need to pursue it
		}
	}
	
	return true;
}

FGoapWorldState UGoapGoalDataAsset::GetDesiredState_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const
{
	FGoapWorldState LegacyDesiredState;

	for (const auto& Pair : DesiredBoolStates)
	{
		LegacyDesiredState.SetBoolState(Pair.Key, Pair.Value);
	}

	for (const auto& Pair : DesiredObjectStates)
	{
		LegacyDesiredState.SetObjectState(Pair.Key, Pair.Value);
	}

	// Preserve backward compatibility for any callsites still using FGoapWorldState-only goal semantics.
	for (const FGoapIntCondition& Condition : IntDesiredConditions)
	{
		if (Condition.StateKey != NAME_None && Condition.Operator == EGoapComparisonOperator::Equal)
		{
			LegacyDesiredState.SetIntState(Condition.StateKey, Condition.Value);
		}
	}

	for (const FGoapFloatCondition& Condition : FloatDesiredConditions)
	{
		if (Condition.StateKey != NAME_None && Condition.Operator == EGoapComparisonOperator::Equal)
		{
			LegacyDesiredState.SetFloatState(Condition.StateKey, Condition.Value);
		}
	}

	return LegacyDesiredState;
}
