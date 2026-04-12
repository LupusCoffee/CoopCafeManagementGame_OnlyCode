// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "../Core/GoapWorldState.h"
#include "GoapGoalDataAsset.h"
#include "GOAPSystem/AI/Controller/MovePriority.h"
#include "GoapActionDataAsset.generated.h"

UENUM(BlueprintType)
enum class EGoapInteractionType : uint8
{
	None UMETA(DisplayName = "Always fail"),
	MoveTo UMETA(DisplayName = "Move to Target"),
	Interact UMETA(DisplayName = "Interract with Target"),
	Wait UMETA(DisplayName = "Wait for Time or Interaction"),
	WaitForAnimation UMETA(DisplayName = "Wait for Animation to Complete"),
	InteractAndWaitForNotify UMETA(DisplayName = "Interact with Target and Wait for Notify"),
	MoveToAndInteract UMETA(DisplayName = "Move to Target and Interact"),
	Custom UMETA(DisplayName = "Override for custom implementation"),
};

UENUM(BlueprintType)
enum class EGoapWaitType : uint8
{
	Time UMETA(DisplayName = "Wait for Time"),
	Interaction UMETA(DisplayName = "Wait for Interaction"),
	TimeOrInteraction UMETA(DisplayName = "Wait for Time or Interaction")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoapActionCompletedWithTags, const FGameplayTagContainer&, EventTags, AActor*, Agent);

/** GOAP action template - READ ONLY at runtime */
UCLASS(BlueprintType)
class GOAPSYSTEM_API UGoapActionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UGoapActionDataAsset();

	/** Broadcasts completion or failure events if bSendTagEvents is enabled */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Events")
	void BroadcastCompletionEvent(AActor* Agent, bool bSuccess);

	/** Broadcasts action start events if bSendTagEvents is enabled */
	UFUNCTION(BlueprintCallable, Category = "GOAP|Events")
	void BroadcastStartEvent(AActor* Agent);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	FText ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Planning")
	float Cost;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Preconditions", meta = (DisplayName = "Float Precondition Checks"))
	TArray<FGoapFloatCondition> FloatPreconditionChecks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Preconditions", meta = (DisplayName = "Int Precondition Checks"))
	TArray<FGoapIntCondition> IntPreconditionChecks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Preconditions", meta = (DisplayName = "Bool Precondition Checks"))
	TArray<FGoapBoolCondition> BoolPreconditionChecks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning")
	FGoapWorldState Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning")
	FGoapWorldState FailureEffects;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	EGoapInteractionType InteractionType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::Wait"))
	EGoapWaitType WaitType = EGoapWaitType::Time;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::MoveTo || InteractionType == EGoapInteractionType::Interact || InteractionType == EGoapInteractionType::InteractAndWaitForNotify || InteractionType == EGoapInteractionType::MoveToAndInteract"))
	FName TargetActorKey;

	/** If true, InteractAndWaitForNotify will move into AcceptanceRadius before calling Interact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::InteractAndWaitForNotify"))
	bool bMoveToTargetBeforeInteractNotify = false;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::Interact || InteractionType == EGoapInteractionType::Wait || InteractionType == EGoapInteractionType::WaitForAnimation || InteractionType == EGoapInteractionType::InteractAndWaitForNotify || InteractionType == EGoapInteractionType::MoveToAndInteract"))
	TObjectPtr<UAnimMontage> AnimationMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	FGameplayTagContainer ActionTags;

	/** Per-action additive cost bias used by tag-based prioritization (negative favors this action). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Planning|Tags")
	float TagCostBias = 0.0f;

	/** If true, this action bypasses component-level blocked action tags. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Planning|Tags")
	bool bIgnoreComponentTagBlocks = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::Wait"))
	float Duration;

	/** If true, the action will fail when timeout occurs. If false, it will succeed. Only applies when WaitType includes interaction. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::Wait && (WaitType == EGoapWaitType::Interaction || WaitType == EGoapWaitType::TimeOrInteraction)"))
	bool bFailOnTimeout = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::MoveTo || InteractionType == EGoapInteractionType::MoveToAndInteract || (InteractionType == EGoapInteractionType::InteractAndWaitForNotify && bMoveToTargetBeforeInteractNotify)"))
	EMovePriority MovePriority = EMovePriority::Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::MoveTo || InteractionType == EGoapInteractionType::MoveToAndInteract || (InteractionType == EGoapInteractionType::InteractAndWaitForNotify && bMoveToTargetBeforeInteractNotify)"))
	float AcceptanceRadius;

	/** Re-issue MoveTo when path-following reports idle/success but agent is still outside AcceptanceRadius. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (EditCondition = "InteractionType == EGoapInteractionType::MoveTo || InteractionType == EGoapInteractionType::MoveToAndInteract || (InteractionType == EGoapInteractionType::InteractAndWaitForNotify && bMoveToTargetBeforeInteractNotify)"))
	bool bRetryMoveWhenDestinationNotReached = true;

	/** Maximum re-issue attempts when destination verification fails. Set to 0 to disable retries. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution", meta = (ClampMin = "0", EditCondition = "InteractionType == EGoapInteractionType::MoveTo || InteractionType == EGoapInteractionType::MoveToAndInteract || (InteractionType == EGoapInteractionType::InteractAndWaitForNotify && bMoveToTargetBeforeInteractNotify)"))
	int32 MaxMoveRecoveryRetries = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events")
	bool bSendTagEvents = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events", meta = (EditCondition = "bSendTagEvents", EditConditionHides))
	FGameplayTagContainer CompletionEventTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events", meta = (EditCondition = "bSendTagEvents", EditConditionHides))
	FGameplayTagContainer StartEventTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events", meta = (EditCondition = "bSendTagEvents", EditConditionHides))
	FGameplayTagContainer FailureEventTags;

	/** Event fired when this action completes successfully (if bSendTagEvents is true) */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGoapActionCompletedWithTags OnActionCompleted;

	/** Event fired when this action fails (if bSendTagEvents is true) */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGoapActionCompletedWithTags OnActionFailed;

	/** Event fired when this action starts (if bSendTagEvents is true) */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGoapActionCompletedWithTags OnActionStarted;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GOAP")
	bool CheckProceduralPrecondition(AActor* Agent, const FGoapWorldState& CurrentState) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GOAP")
	float GetDynamicCost(AActor* Agent, const FGoapWorldState& CurrentState) const;

	/** Builds a precondition world state from the condition checks arrays for planning */
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	FGoapWorldState BuildPreconditionState() const;

protected:
	virtual bool CheckProceduralPrecondition_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const;
	virtual float GetDynamicCost_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const;
};
