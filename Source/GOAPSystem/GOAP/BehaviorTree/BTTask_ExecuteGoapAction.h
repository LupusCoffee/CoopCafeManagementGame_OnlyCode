#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteGoapActionMemory.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_ExecuteGoapAction.generated.h"

class UGoapComponent;
class UGoapActionDataAsset;
class ACharacter;
class AAIController;
class UBehaviorTreeComponent;
struct FGoapActionInstance;

/** BT task that executes the current GOAP action */
UCLASS()
class GOAPSYSTEM_API UBTTask_ExecuteGoapAction : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ExecuteGoapAction();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	EBTNodeResult::Type ExecuteMoveTo(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action);
	EBTNodeResult::Type ExecuteInteract(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action);
	EBTNodeResult::Type ExecuteWait(UGoapComponent* GoapComp, ACharacter* Agent, const FGoapActionInstance& ActionInstance, uint8* NodeMemory);
	EBTNodeResult::Type ExecuteWaitForAnimation(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action);
	EBTNodeResult::Type ExecuteInteractAndWaitForNotify(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory);
	EBTNodeResult::Type ExecuteMoveToAndInteract(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory);
	EBTNodeResult::Type StartOrResumeMovement(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, const TCHAR* Context, bool bIsRetry);
	EBTNodeResult::Type HandleMovementArrived(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, AActor* TargetActor, const TCHAR* Context);
	EBTNodeResult::Type HandleMovementNotAtDestination(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, float DistanceToTarget, const TCHAR* Context);
	void MonitorMovementProgress(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, const TCHAR* Context);
	
	/** Helper to find target actor by key (checks world state first, then subsystem lookup) */
	AActor* FindTargetActor(UGoapComponent* GoapComp, AActor* Agent, FName TargetKey) const;
	bool IsMovementAction(const UGoapActionDataAsset* Action) const;
	bool IsAgentAtActionDestination(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, AActor*& OutTargetActor, float& OutDistanceToTarget) const;
	
	virtual uint16 GetInstanceMemorySize() const override;

private:
	FBTExecuteGoapActionMemory* GetTaskMemory(uint8* NodeMemory) const;
	void ResetCachedMoveState();

	/** Cached controller for move completed delegate */
	UPROPERTY()
	TWeakObjectPtr<AAIController> CachedController;
	
	/** Cached behavior tree component for delegate callback */
	UPROPERTY()
	TWeakObjectPtr<UBehaviorTreeComponent> CachedBTComp;

	/** Tracks the specific GOAP move request we started so unrelated move completions can be ignored */
	FAIRequestID CachedMoveRequestId = FAIRequestID::InvalidRequest;
	
	/** Callback for when AI movement completes */
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
