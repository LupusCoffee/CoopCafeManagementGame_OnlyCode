// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/GoapWorldState.h"
#include "../GoapActionDataAsset.h"
#include "../GoapGoalDataAsset.h"
#include "DataAssetStructs.generated.h"

/** Runtime instance of a GOAP action - mutable per AI */
USTRUCT(BlueprintType)
struct GOAPSYSTEM_API FGoapActionInstance
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Action")
    TObjectPtr<UGoapActionDataAsset> SourceDataAsset;

    // Mutable planning data
    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    FGoapWorldState Preconditions;

    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    FGoapWorldState Effects;

    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    float RuntimeCost;

    // Cached execution data
    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    FText ActionName;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    EGoapInteractionType InteractionType;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    FName TargetActorKey;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    TObjectPtr<UAnimMontage> AnimationMontage;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    FGameplayTagContainer ActionTags;

    UPROPERTY(BlueprintReadOnly, Category = "Planning|Tags")
    float TagCostBias;

    UPROPERTY(BlueprintReadOnly, Category = "Planning|Tags")
    bool bIgnoreComponentTagBlocks;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    float Duration;

    UPROPERTY(BlueprintReadWrite, Category = "Execution")
    float RuntimeDuration;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    float AcceptanceRadius;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    bool bFailOnTimeout;

    UPROPERTY(BlueprintReadOnly, Category = "Execution")
    EMovePriority MovePriority = EMovePriority::Normal;

    FGoapActionInstance()
        : RuntimeCost(1.0f)
        , InteractionType(EGoapInteractionType::None)
        , Duration(0.0f)
        , RuntimeDuration(0.0f)
        , AcceptanceRadius(100.0f)
        , TagCostBias(0.0f)
        , bIgnoreComponentTagBlocks(false)
        , bFailOnTimeout(true)
        , MovePriority(EMovePriority::Normal)
    {
    }

    static FGoapActionInstance CreateFromDataAsset(UGoapActionDataAsset* DataAsset);
    void RefreshFromSource();
    bool IsValid() const { return SourceDataAsset.Get() != nullptr; }
    float GetCost() const { return RuntimeCost; }
    bool CheckProceduralPrecondition(AActor* Agent, const FGoapWorldState& CurrentState) const;
};

/** Runtime instance of a GOAP goal - mutable per AI */
USTRUCT(BlueprintType)
struct GOAPSYSTEM_API FGoapGoalInstance
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Goal")
    TObjectPtr<UGoapGoalDataAsset> SourceDataAsset;

    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    TMap<FName, bool> DesiredBoolStates;

    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    TMap<FName, TObjectPtr<UObject>> DesiredObjectStates;

    /** Operator-based desired int states copied from goal data asset. */
    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    TArray<FGoapIntCondition> IntDesiredConditions;

    /** Operator-based desired float states copied from goal data asset. */
    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    TArray<FGoapFloatCondition> FloatDesiredConditions;

    UPROPERTY(BlueprintReadWrite, Category = "Planning")
    float RuntimePriority;

    UPROPERTY(BlueprintReadOnly, Category = "Goal")
    FText GoalName;

    FGoapGoalInstance() : RuntimePriority(1.0f) {}

    static FGoapGoalInstance CreateFromDataAsset(UGoapGoalDataAsset* DataAsset);
    void RefreshFromSource();
    bool IsValid() const { return SourceDataAsset.Get() != nullptr; }
    float GetPriority(AActor* Agent, const FGoapWorldState& CurrentState) const;
    bool IsValidGoal(AActor* Agent, const FGoapWorldState& CurrentState) const;
    FGoapWorldState GetDesiredState(AActor* Agent, const FGoapWorldState& CurrentState) const;

    /** Runtime-safe goal completion check that supports per-goal int/float operators. */
    bool SatisfiesDesiredState(const FGoapWorldState& CurrentState) const;

    /** Runtime-safe heuristic distance to goal (mismatch count) supporting per-goal operators. */
    int32 GetDesiredStateDistance(const FGoapWorldState& CurrentState) const;
};
