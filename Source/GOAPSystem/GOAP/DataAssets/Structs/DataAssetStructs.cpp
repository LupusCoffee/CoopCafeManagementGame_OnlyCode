#include "DataAssetStructs.h"

#include "GOAPSystem/GOAP/DataAssets/GoapGoalDataAsset.h"

FGoapActionInstance FGoapActionInstance::CreateFromDataAsset(UGoapActionDataAsset* DataAsset)
{
    FGoapActionInstance Instance;
    
    if (!DataAsset)
    {
        return Instance;
    }
    
    Instance.SourceDataAsset = DataAsset;
    Instance.Preconditions = DataAsset->BuildPreconditionState();
    Instance.Effects = DataAsset->Effects;
    Instance.RuntimeCost = DataAsset->Cost;
    Instance.ActionName = DataAsset->ActionName;
    Instance.InteractionType = DataAsset->InteractionType;
    Instance.TargetActorKey = DataAsset->TargetActorKey;
    Instance.AnimationMontage = DataAsset->AnimationMontage;
    Instance.ActionTags = DataAsset->ActionTags;
    Instance.TagCostBias = DataAsset->TagCostBias;
    Instance.bIgnoreComponentTagBlocks = DataAsset->bIgnoreComponentTagBlocks;
    Instance.Duration = DataAsset->Duration;
    Instance.RuntimeDuration = DataAsset->Duration;  // Initialize runtime duration from template
    Instance.AcceptanceRadius = DataAsset->AcceptanceRadius;
    Instance.bFailOnTimeout = DataAsset->bFailOnTimeout;
    Instance.MovePriority = DataAsset->MovePriority;

    return Instance;
}

void FGoapActionInstance::RefreshFromSource()
{
    if (!SourceDataAsset)
    {
        return;
    }

    ActionName = SourceDataAsset->ActionName;
    InteractionType = SourceDataAsset->InteractionType;
    TargetActorKey = SourceDataAsset->TargetActorKey;
    AnimationMontage = SourceDataAsset->AnimationMontage;
    ActionTags = SourceDataAsset->ActionTags;
    TagCostBias = SourceDataAsset->TagCostBias;
    bIgnoreComponentTagBlocks = SourceDataAsset->bIgnoreComponentTagBlocks;
    Duration = SourceDataAsset->Duration;
    RuntimeDuration = SourceDataAsset->Duration;  // Reset runtime duration to template value
    AcceptanceRadius = SourceDataAsset->AcceptanceRadius;
    bFailOnTimeout = SourceDataAsset->bFailOnTimeout;
    MovePriority = SourceDataAsset->MovePriority;
}

bool FGoapActionInstance::CheckProceduralPrecondition(AActor* Agent, const FGoapWorldState& CurrentState) const
{
    return SourceDataAsset ? SourceDataAsset->CheckProceduralPrecondition(Agent, CurrentState) : true;
}

FGoapGoalInstance FGoapGoalInstance::CreateFromDataAsset(UGoapGoalDataAsset* DataAsset)
{
    FGoapGoalInstance Instance;
    
    if (!DataAsset)
    {
        return Instance;
    }

    Instance.SourceDataAsset = DataAsset;
    Instance.DesiredBoolStates = DataAsset->DesiredBoolStates;
    Instance.DesiredObjectStates = DataAsset->DesiredObjectStates;
    Instance.IntDesiredConditions = DataAsset->IntDesiredConditions;
    Instance.FloatDesiredConditions = DataAsset->FloatDesiredConditions;
    Instance.RuntimePriority = DataAsset->BasePriority;
    Instance.GoalName = DataAsset->GoalName;

    return Instance;
}

void FGoapGoalInstance::RefreshFromSource()
{
    if (SourceDataAsset)
    {
        GoalName = SourceDataAsset->GoalName;
        DesiredBoolStates = SourceDataAsset->DesiredBoolStates;
        DesiredObjectStates = SourceDataAsset->DesiredObjectStates;
        IntDesiredConditions = SourceDataAsset->IntDesiredConditions;
        FloatDesiredConditions = SourceDataAsset->FloatDesiredConditions;
    }
}

float FGoapGoalInstance::GetPriority(AActor* Agent, const FGoapWorldState& CurrentState) const
{
    return SourceDataAsset ? SourceDataAsset->GetPriority(Agent, CurrentState) : RuntimePriority;
}

bool FGoapGoalInstance::IsValidGoal(AActor* Agent, const FGoapWorldState& CurrentState) const
{
    return SourceDataAsset ? SourceDataAsset->IsValid(Agent, CurrentState) : false;
}

FGoapWorldState FGoapGoalInstance::GetDesiredState(AActor* Agent, const FGoapWorldState& CurrentState) const
{
    if (SourceDataAsset)
    {
        return SourceDataAsset->GetDesiredState(Agent, CurrentState);
    }

    FGoapWorldState LegacyDesiredState;
    for (const auto& Pair : DesiredBoolStates)
    {
        LegacyDesiredState.SetBoolState(Pair.Key, Pair.Value);
    }
    for (const auto& Pair : DesiredObjectStates)
    {
        LegacyDesiredState.SetObjectState(Pair.Key, Pair.Value);
    }
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

bool FGoapGoalInstance::SatisfiesDesiredState(const FGoapWorldState& CurrentState) const
{
    // Bool/Object desired states keep exact semantics.
    for (const auto& Pair : DesiredBoolStates)
    {
        if (CurrentState.GetBoolState(Pair.Key, !Pair.Value) != Pair.Value)
        {
            return false;
        }
    }

    for (const auto& Pair : DesiredObjectStates)
    {
        if (CurrentState.GetObjectState(Pair.Key) != Pair.Value)
        {
            return false;
        }
    }

    for (const FGoapIntCondition& Condition : IntDesiredConditions)
    {
        if (!Condition.Evaluate(CurrentState))
        {
            return false;
        }
    }

    for (const FGoapFloatCondition& Condition : FloatDesiredConditions)
    {
        if (!Condition.Evaluate(CurrentState))
        {
            return false;
        }
    }

    return true;
}

int32 FGoapGoalInstance::GetDesiredStateDistance(const FGoapWorldState& CurrentState) const
{
    int32 Distance = 0;

    for (const auto& Pair : DesiredBoolStates)
    {
        if (CurrentState.GetBoolState(Pair.Key, !Pair.Value) != Pair.Value)
        {
            ++Distance;
        }
    }

    for (const auto& Pair : DesiredObjectStates)
    {
        if (CurrentState.GetObjectState(Pair.Key) != Pair.Value)
        {
            ++Distance;
        }
    }

    for (const FGoapIntCondition& Condition : IntDesiredConditions)
    {
        if (!Condition.Evaluate(CurrentState))
        {
            ++Distance;
        }
    }

    for (const FGoapFloatCondition& Condition : FloatDesiredConditions)
    {
        if (!Condition.Evaluate(CurrentState))
        {
            ++Distance;
        }
    }

    return Distance;
}

