// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTTask_ExecuteGoapAction.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Character.h"
#include "GOAPSystem/AI/Interface/Interractable.h"
#include "GOAPSystem/AI/Utils/AIComponentUtils.h"
#include "Navigation/PathFollowingComponent.h"
#include "../Subsystems/GoapWorldSubsystem.h"
#include "../Components/GoapComponent.h"
#include "../DataAssets/GoapActionDataAsset.h"
#include "GOAPSystem/AI/Controller/BaseAiController.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_ExecuteGoapAction::UBTTask_ExecuteGoapAction()
{
	NodeName = "Execute GOAP Action";
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

FBTExecuteGoapActionMemory* UBTTask_ExecuteGoapAction::GetTaskMemory(uint8* NodeMemory) const
{
	return reinterpret_cast<FBTExecuteGoapActionMemory*>(NodeMemory);
}

void UBTTask_ExecuteGoapAction::ResetCachedMoveState()
{
	if (CachedController.IsValid())
	{
		CachedController->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_ExecuteGoapAction::OnMoveCompleted);
		CachedController.Reset();
	}

	CachedMoveRequestId = FAIRequestID::InvalidRequest;
}

bool UBTTask_ExecuteGoapAction::IsMovementAction(const UGoapActionDataAsset* Action) const
{
	return Action &&
		(Action->InteractionType == EGoapInteractionType::MoveTo ||
		 Action->InteractionType == EGoapInteractionType::MoveToAndInteract ||
		 (Action->InteractionType == EGoapInteractionType::InteractAndWaitForNotify && Action->bMoveToTargetBeforeInteractNotify));
}

bool UBTTask_ExecuteGoapAction::IsAgentAtActionDestination(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, AActor*& OutTargetActor, float& OutDistanceToTarget) const
{
	OutTargetActor = FindTargetActor(GoapComp, Agent, Action ? Action->TargetActorKey : NAME_None);
	OutDistanceToTarget = TNumericLimits<float>::Max();

	if (!OutTargetActor || !Agent || !Action)
	{
		return false;
	}

	const FVector AgentLocation = Agent->GetActorLocation();
	const FVector TargetLocation = OutTargetActor->GetActorLocation();

	FVector BoundsOrigin = FVector::ZeroVector;
	FVector BoundsExtent = FVector::ZeroVector;
	OutTargetActor->GetActorBounds(true, BoundsOrigin, BoundsExtent);

	const float DistanceToTargetCenter = FVector::Dist2D(AgentLocation, TargetLocation);
	const float BoundsRadius = FVector2D(BoundsExtent.X, BoundsExtent.Y).Size();
	if (BoundsRadius > KINDA_SMALL_NUMBER)
	{
		// Measure to the surface of the target bounds so arrival checks match actor-goal movement semantics.
		OutDistanceToTarget = FMath::Max(0.0f, DistanceToTargetCenter - BoundsRadius);
	}
	else
	{
		OutDistanceToTarget = DistanceToTargetCenter;
	}

	return OutDistanceToTarget <= Action->AcceptanceRadius;
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::HandleMovementArrived(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, AActor* TargetActor, const TCHAR* Context)
{
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	if (Memory)
	{
		Memory->bMoveRequestActive = false;
	}

	if (!GoapComp || !Agent || !Action)
	{
		if (GoapComp)
		{
			GoapComp->FailCurrentAction();
		}
		return EBTNodeResult::Failed;
	}

	switch (Action->InteractionType)
	{
	case EGoapInteractionType::MoveTo:
		UE_LOG(LogTemp, Log, TEXT("%s: Destination verified - completing MoveTo action"), Context);
		GoapComp->CompleteCurrentAction();
		return EBTNodeResult::Succeeded;

	case EGoapInteractionType::MoveToAndInteract:
		if (!TargetActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: MoveToAndInteract target is no longer valid"), Context);
			GoapComp->FailCurrentAction();
			return EBTNodeResult::Failed;
		}

		if (UKismetSystemLibrary::DoesImplementInterface(TargetActor, UGOAPInteractable::StaticClass()))
		{
			IGOAPInteractable::Execute_Interact(TargetActor, Agent);
		}

		if (Action->AnimationMontage && Agent->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Agent->GetMesh()->GetAnimInstance())
			{
				const float MontageLength = AnimInstance->Montage_Play(Action->AnimationMontage);
				if (MontageLength > 0.0f)
				{
					UE_LOG(LogTemp, Log, TEXT("%s: Movement finished, playing interaction animation '%s'"), Context, *Action->AnimationMontage->GetName());
					if (Memory)
					{
						Memory->bMovementCompleted = true;
					}
					return EBTNodeResult::InProgress;
				}
			}
		}

		GoapComp->CompleteCurrentAction();
		return EBTNodeResult::Succeeded;

	case EGoapInteractionType::InteractAndWaitForNotify:
		if (!Action->bMoveToTargetBeforeInteractNotify)
		{
			GoapComp->CompleteCurrentAction();
			return EBTNodeResult::Succeeded;
		}

		if (!TargetActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: InteractAndWaitForNotify target is no longer valid"), Context);
			GoapComp->FailCurrentAction();
			return EBTNodeResult::Failed;
		}

		if (UKismetSystemLibrary::DoesImplementInterface(TargetActor, UGOAPInteractable::StaticClass()))
		{
			IGOAPInteractable::Execute_Interact(TargetActor, Agent);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Target actor '%s' does not implement Interractable interface"), Context, *TargetActor->GetName());
		}

		if (Action->AnimationMontage && Agent->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Agent->GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(Action->AnimationMontage);
			}
		}

		if (Memory)
		{
			Memory->bWaitingForNotify = true;
			Memory->bInteractionReceived = false;
		}

		UE_LOG(LogTemp, Log, TEXT("%s: Movement finished, started interaction and waiting for notify"), Context);
		return EBTNodeResult::InProgress;

	default:
		GoapComp->CompleteCurrentAction();
		return EBTNodeResult::Succeeded;
	}
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::HandleMovementNotAtDestination(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, float DistanceToTarget, const TCHAR* Context)
{
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	const int32 RecoveryRetryLimit = Action ? FMath::Max(0, Action->MaxMoveRecoveryRetries) : 0;
	const bool bAllowRetry = Action && Action->bRetryMoveWhenDestinationNotReached;

	if (!bAllowRetry)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Destination not reached (distance: %.2f) and recovery retries are disabled. Failing action."), Context, DistanceToTarget);
		if (Memory)
		{
			Memory->bMoveRequestActive = false;
		}
		ResetCachedMoveState();
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}

	if (Memory && Memory->MovementRetryCount >= RecoveryRetryLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Destination not reached (distance: %.2f). Recovery retry limit (%d) reached; failing action."), Context, DistanceToTarget, RecoveryRetryLimit);
		Memory->bMoveRequestActive = false;
		ResetCachedMoveState();
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}

	if (Memory)
	{
		++Memory->MovementRetryCount;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s: Destination not reached (distance: %.2f). Reissuing move attempt %d/%d."), Context, DistanceToTarget, Memory ? static_cast<int32>(Memory->MovementRetryCount) : 1, RecoveryRetryLimit);
	return StartOrResumeMovement(OwnerComp, GoapComp, Agent, Action, NodeMemory, Context, true);
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::StartOrResumeMovement(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, const TCHAR* Context, bool bIsRetry)
{
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	if (Memory)
	{
		Memory->bMoveRequestActive = false;
		if (!bIsRetry)
		{
			Memory->MovementRetryCount = 0;
		}
	}

	AActor* TargetActor = nullptr;
	float DistanceToTarget = 0.0f;
	if (IsAgentAtActionDestination(GoapComp, Agent, Action, TargetActor, DistanceToTarget))
	{
		UE_LOG(LogTemp, Log, TEXT("%s: Already within acceptance radius (distance %.2f <= %.2f)"), Context, DistanceToTarget, Action->AcceptanceRadius);
		ResetCachedMoveState();
		return HandleMovementArrived(OwnerComp, GoapComp, Agent, Action, NodeMemory, TargetActor, Context);
	}

	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No actor found for key '%s'"), Context, Action ? *Action->TargetActorKey.ToString() : TEXT("None"));
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}

	ABaseAiController* AIController = Cast<ABaseAiController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}

	ResetCachedMoveState();
	CachedBTComp = &OwnerComp;
	CachedController = AIController;
	AIController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_ExecuteGoapAction::OnMoveCompleted);

	FAIMoveRequest MoveRequest(TargetActor);
	MoveRequest.SetAcceptanceRadius(Action->AcceptanceRadius);
	MoveRequest.SetReachTestIncludesAgentRadius(false);
	MoveRequest.SetReachTestIncludesGoalRadius(false);

	const FPathFollowingRequestResult MoveResult = AIController->RequestMoveWithPriority(MoveRequest, Action->MovePriority);
	if (MoveResult.Code == EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		CachedMoveRequestId = MoveResult.MoveId;
		if (Memory)
		{
			Memory->bMoveRequestActive = true;
		}

		UE_LOG(LogTemp, Log, TEXT("%s: Started movement to %s (distance: %.2f, priority: %d, retry: %d)"), Context, *TargetActor->GetName(), DistanceToTarget, static_cast<int32>(Action->MovePriority), Memory ? static_cast<int32>(Memory->MovementRetryCount) : 0);
		return EBTNodeResult::InProgress;
	}

	if (MoveResult.Code == EPathFollowingRequestResult::Type::AlreadyAtGoal)
	{
		UE_LOG(LogTemp, Log, TEXT("%s: Controller reported already at goal; verifying arrival"), Context);
		ResetCachedMoveState();

		AActor* VerifiedTarget = nullptr;
		float VerifiedDistance = 0.0f;
		if (IsAgentAtActionDestination(GoapComp, Agent, Action, VerifiedTarget, VerifiedDistance))
		{
			return HandleMovementArrived(OwnerComp, GoapComp, Agent, Action, NodeMemory, VerifiedTarget, Context);
		}

		return HandleMovementNotAtDestination(OwnerComp, GoapComp, Agent, Action, NodeMemory, VerifiedDistance, Context);
	}

	ResetCachedMoveState();
	UE_LOG(LogTemp, Warning, TEXT("%s: Failed to start movement (code: %d)"), Context, static_cast<int32>(MoveResult.Code));
	GoapComp->FailCurrentAction();
	return EBTNodeResult::Failed;
}

void UBTTask_ExecuteGoapAction::MonitorMovementProgress(UBehaviorTreeComponent& OwnerComp, UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory, const TCHAR* Context)
{
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	if (!Memory)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Missing task memory while monitoring movement"), Context);
		GoapComp->FailCurrentAction();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* TargetActor = nullptr;
	float DistanceToTarget = 0.0f;

	if (!Memory->bMoveRequestActive)
	{
		if (IsAgentAtActionDestination(GoapComp, Agent, Action, TargetActor, DistanceToTarget))
		{
			UE_LOG(LogTemp, Log, TEXT("%s: No active move request, but destination is reached (distance: %.2f)"), Context, DistanceToTarget);
			ResetCachedMoveState();
			const EBTNodeResult::Type CompletionResult = HandleMovementArrived(OwnerComp, GoapComp, Agent, Action, NodeMemory, TargetActor, Context);
			if (CompletionResult != EBTNodeResult::InProgress)
			{
				FinishLatentTask(OwnerComp, CompletionResult);
			}
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("%s: Agent is idle with no active move request and is still %.2f units away. Attempting movement recovery."), Context, DistanceToTarget);
		const EBTNodeResult::Type RecoveryResult = HandleMovementNotAtDestination(OwnerComp, GoapComp, Agent, Action, NodeMemory, DistanceToTarget, Context);
		if (RecoveryResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, RecoveryResult);
		}
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	UPathFollowingComponent* PathFollowingComponent = AIController ? AIController->GetPathFollowingComponent() : nullptr;
	const EPathFollowingStatus::Type PathStatus = PathFollowingComponent ? PathFollowingComponent->GetStatus() : EPathFollowingStatus::Idle;
	if (PathStatus != EPathFollowingStatus::Idle)
	{
		return;
	}

	if (IsAgentAtActionDestination(GoapComp, Agent, Action, TargetActor, DistanceToTarget))
	{
		UE_LOG(LogTemp, Log, TEXT("%s: Path following is idle, but destination is already reached (distance: %.2f)"), Context, DistanceToTarget);
		ResetCachedMoveState();
		const EBTNodeResult::Type CompletionResult = HandleMovementArrived(OwnerComp, GoapComp, Agent, Action, NodeMemory, TargetActor, Context);
		if (CompletionResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(OwnerComp, CompletionResult);
		}
		return;
	}

	const EBTNodeResult::Type RetryResult = HandleMovementNotAtDestination(OwnerComp, GoapComp, Agent, Action, NodeMemory, DistanceToTarget, Context);
	if (RetryResult != EBTNodeResult::InProgress)
	{
		FinishLatentTask(OwnerComp, RetryResult);
	}
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Cache the BT component for delegate callback
	CachedBTComp = &OwnerComp;
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Missing AIController"));
		return EBTNodeResult::Failed;
	}

	if (!AIController->HasAuthority())
	{
		return EBTNodeResult::Failed;
	}
	
	ACharacter* Agent = Cast<ACharacter>(AIController->GetPawn());
	if (!Agent)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Missing/invalid controlled pawn as ACharacter"));
		return EBTNodeResult::Failed;
	}
	
	UGoapComponent* GoapComp = FAIComponentUtils::GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Missing GOAP component on agent '%s'"), *Agent->GetName());
		return EBTNodeResult::Failed;
	}
	
	FGoapActionInstance CurrentAction = GoapComp->GetCurrentAction();
	if (!CurrentAction.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Current GOAP action is invalid"));
	    return EBTNodeResult::Failed;
	}
	
	UGoapActionDataAsset* ActionTemplate = CurrentAction.SourceDataAsset;
	if (!ActionTemplate)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Current action has no source data asset"));
		return EBTNodeResult::Failed;
	}
	
	// Check if preconditions are still satisfied before executing
	const FGoapWorldState& CurrentWorldState = GoapComp->GetWorldState();
	
	// Check preconditions (validates all condition checks)
	if (!ActionTemplate->CheckProceduralPrecondition(Agent, CurrentWorldState))
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Procedural precondition failed for action '%s'"), *ActionTemplate->ActionName.ToString());
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}
	
	switch (CurrentAction.InteractionType)
	{
	    case EGoapInteractionType::MoveTo:
	        return ExecuteMoveTo(GoapComp, Agent, ActionTemplate);
	    
	    case EGoapInteractionType::Interact:
	        return ExecuteInteract(GoapComp, Agent, ActionTemplate);
	    
	    case EGoapInteractionType::Wait:
	        return ExecuteWait(GoapComp, Agent, CurrentAction, NodeMemory);
	    
	    case EGoapInteractionType::WaitForAnimation:
	        return ExecuteWaitForAnimation(GoapComp, Agent, ActionTemplate);
	    
	    case EGoapInteractionType::InteractAndWaitForNotify:
	        return ExecuteInteractAndWaitForNotify(GoapComp, Agent, ActionTemplate, NodeMemory);
	    
	    case EGoapInteractionType::MoveToAndInteract:
	        return ExecuteMoveToAndInteract(GoapComp, Agent, ActionTemplate, NodeMemory);
	    
	    case EGoapInteractionType::None:
	        GoapComp->CompleteCurrentAction();
	        return EBTNodeResult::Succeeded;
	    
	    case EGoapInteractionType::Custom:
	    	UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Interaction type Custom is not implemented for action '%s'"), *ActionTemplate->ActionName.ToString());
	        return EBTNodeResult::Failed;
	}
	
	UE_LOG(LogTemp, Error, TEXT("ExecuteTask: Unhandled interaction type %d for action '%s'"), static_cast<int32>(CurrentAction.InteractionType), *ActionTemplate->ActionName.ToString());
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Clean up delegates before aborting to prevent stale callbacks
	if (FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory))
	{
		Memory->bMoveRequestActive = false;
	}
	ResetCachedMoveState();
	
	CachedBTComp.Reset();
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("AbortTask: Missing AIController while aborting GOAP task"));
		return EBTNodeResult::Aborted;
	}

	AActor* Agent = AIController->GetPawn();
	if (UGoapComponent* GoapComp = FAIComponentUtils::GetGoapComponent(Agent))
	{
		// If request-replan already cleared the active action/plan, this is an intentional abort.
		if (GoapComp->HasActiveAction() && GoapComp->GetAgentState() == EGoapAgentState::ExecutingPlan)
		{
			GoapComp->FailCurrentAction();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AbortTask: Missing GOAP component while aborting task"));
	}
	
	return EBTNodeResult::Aborted;
}

void UBTTask_ExecuteGoapAction::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("TickTask: Missing AIController"));
		return;
	}

	if (!AIController->HasAuthority())
	{
		return;
	}
	
	ACharacter* Agent = Cast<ACharacter>(AIController->GetPawn());
	if (!Agent)
	{
		UE_LOG(LogTemp, Error, TEXT("TickTask: Missing/invalid controlled pawn as ACharacter"));
		return;
	}
	
	UGoapComponent* GoapComp = FAIComponentUtils::GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Error, TEXT("TickTask: Missing GOAP component on agent '%s'"), *Agent->GetName());
		return;
	}
	
	FGoapActionInstance CurrentAction = GoapComp->GetCurrentAction();
	if (!CurrentAction.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("TickTask: Current action became invalid during execution"));
		GoapComp->FailCurrentAction();
	    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	    return;
	}
	
	UGoapActionDataAsset* ActionTemplate = CurrentAction.SourceDataAsset;
	if (!ActionTemplate)
	{
		UE_LOG(LogTemp, Error, TEXT("TickTask: Current action missing source data asset"));
		GoapComp->FailCurrentAction();
	    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	    return;
	}
	
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	
	switch (CurrentAction.InteractionType)
	{
	    case EGoapInteractionType::MoveTo:
	    	if (IsMovementAction(ActionTemplate))
	    	{
	    		MonitorMovementProgress(OwnerComp, GoapComp, Agent, ActionTemplate, NodeMemory, TEXT("TickTask MoveTo"));
	    	}
	        break;
	    
	    case EGoapInteractionType::Wait:
		if (Memory && Memory->bIsWaiting)
		{
			// Check for interaction completion (SUCCESS)
			if (Memory->bWaitingForInteraction && Memory->bInteractionReceived)
			{
				UE_LOG(LogTemp, Warning, TEXT("Wait completed via INTERACTION - SUCCESS"));
				Memory->bIsWaiting = false;
				Memory->bTimedOut = false;
				
				// Stop any playing animation if wait completes early
				if (ActionTemplate->AnimationMontage)
				{
					USkeletalMeshComponent* Mesh = Agent->GetMesh();
					UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
					if (AnimInstance && AnimInstance->Montage_IsPlaying(ActionTemplate->AnimationMontage))
					{
						AnimInstance->Montage_Stop(0.25f, ActionTemplate->AnimationMontage);
						UE_LOG(LogTemp, Log, TEXT("Wait: Stopped animation early due to interaction completion"));
					}
				}
				
				GoapComp->CompleteCurrentAction();
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				break;
			}
			
			// Check if animation finished early (only for time-based waits)
			if (ActionTemplate->AnimationMontage && 
				(ActionTemplate->WaitType == EGoapWaitType::Time || ActionTemplate->WaitType == EGoapWaitType::TimeOrInteraction))
			{
				USkeletalMeshComponent* Mesh = Agent->GetMesh();
				UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
				if (AnimInstance && !AnimInstance->Montage_IsPlaying(ActionTemplate->AnimationMontage))
				{
					// Animation finished but wait time not expired - just let timer continue without animation
					UE_LOG(LogTemp, Log, TEXT("Wait: Animation finished early, continuing to wait for time"));
				}
			}
        
			// Check for time completion (if applicable)
			if (ActionTemplate->WaitType == EGoapWaitType::Time || 
				ActionTemplate->WaitType == EGoapWaitType::TimeOrInteraction)
			{
				Memory->WaitTimeRemaining -= DeltaSeconds;
            
				if (Memory->WaitTimeRemaining <= 0.0f)
				{
					Memory->bIsWaiting = false;
					
					// Stop any playing animation if wait time expires
					if (ActionTemplate->AnimationMontage)
					{
						USkeletalMeshComponent* Mesh = Agent->GetMesh();
						UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
						if (AnimInstance && AnimInstance->Montage_IsPlaying(ActionTemplate->AnimationMontage))
						{
							AnimInstance->Montage_Stop(0.25f, ActionTemplate->AnimationMontage);
							UE_LOG(LogTemp, Log, TEXT("Wait: Stopped animation due to wait time expiration"));
						}
				}
				
				// If waiting for interaction and timed out, check if we should fail or succeed
				if (Memory->bWaitingForInteraction && !Memory->bInteractionReceived)
				{
					Memory->bTimedOut = true;
					
					if (GoapComp->OnActionTimeout.IsBound())
					{
						GoapComp->OnActionTimeout.Broadcast(ActionTemplate, GoapComp->GetWorldState());
					}
					
					// Check if timeout should fail or succeed based on action configuration
					if (CurrentAction.bFailOnTimeout)
					{
						UE_LOG(LogTemp, Warning, TEXT("Wait TIMED OUT - failing action and replanning (bFailOnTimeout=true)"));
						// FailCurrentAction will apply failure effects automatically
						GoapComp->FailCurrentAction();
						FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Wait TIMED OUT - but succeeding anyway (bFailOnTimeout=false)"));
						// Complete action successfully despite timeout
						GoapComp->CompleteCurrentAction();
						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					}
				}
				else
				{
					// Normal time-based wait completed successfully
					UE_LOG(LogTemp, Warning, TEXT("Wait completed via TIME - SUCCESS"));
					GoapComp->CompleteCurrentAction();
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				}
			}
			}
		}
		break;
	    
	    case EGoapInteractionType::Interact:
	        if (ActionTemplate->AnimationMontage)
	        {
	        	USkeletalMeshComponent* Mesh = Agent->GetMesh();
	            UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
	            if (AnimInstance && !AnimInstance->Montage_IsPlaying(ActionTemplate->AnimationMontage))
	            {
	                GoapComp->CompleteCurrentAction();
	                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	            }
	        }
	        break;
	    
	    case EGoapInteractionType::WaitForAnimation:
	        if (ActionTemplate->AnimationMontage)
	        {
	        	USkeletalMeshComponent* Mesh = Agent->GetMesh();
	            UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
	            if (AnimInstance && !AnimInstance->Montage_IsPlaying(ActionTemplate->AnimationMontage))
	            {
	                GoapComp->CompleteCurrentAction();
	                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	            }
	        }
	        break;
	    
	    case EGoapInteractionType::InteractAndWaitForNotify:
		if (ActionTemplate->bMoveToTargetBeforeInteractNotify && Memory && !Memory->bWaitingForNotify)
		{
			MonitorMovementProgress(OwnerComp, GoapComp, Agent, ActionTemplate, NodeMemory, TEXT("TickTask InteractAndWaitForNotify"));
			break;
		}
		if (Memory && Memory->bWaitingForNotify)
		{
			// Check if interaction notification was received
			if (Memory->bInteractionReceived)
			{
				UE_LOG(LogTemp, Warning, TEXT("InteractAndWaitForNotify completed via NOTIFICATION - SUCCESS"));
				Memory->bWaitingForNotify = false;
				GoapComp->CompleteCurrentAction();
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
		break;
	    
	    case EGoapInteractionType::MoveToAndInteract:
		if (Memory && !Memory->bMovementCompleted)
		{
			MonitorMovementProgress(OwnerComp, GoapComp, Agent, ActionTemplate, NodeMemory, TEXT("TickTask MoveToAndInteract"));
			break;
		}
		if (Memory && Memory->bMovementCompleted)
		{
			// Movement is complete, check if animation is done
			if (ActionTemplate->AnimationMontage)
			{
				USkeletalMeshComponent* Mesh = Agent->GetMesh();
				UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
				if (AnimInstance && !AnimInstance->Montage_IsPlaying(ActionTemplate->AnimationMontage))
				{
					UE_LOG(LogTemp, Warning, TEXT("MoveToAndInteract: Animation completed - SUCCESS"));
					GoapComp->CompleteCurrentAction();
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				}
			}
			else
			{
				// No animation specified, complete immediately after movement
				GoapComp->CompleteCurrentAction();
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
		break;
	    
        case EGoapInteractionType::None:
        case EGoapInteractionType::Custom:
	        break;
	}
}

AActor* UBTTask_ExecuteGoapAction::FindTargetActor(UGoapComponent* GoapComp, AActor* Agent, FName TargetKey) const
{
	if (!GoapComp || !Agent || TargetKey.IsNone())
		return nullptr;

	// First try to get from world state
	AActor* TargetActor = GoapComp->GetWorldState().GetActorState(TargetKey);
	
	// Fall back to tag-based lookup via subsystem
	if (!TargetActor)
	{
		if (UWorld* World = Agent->GetWorld())
		{
			if (UGoapWorldSubsystem* GoapWorldSubsystem = World->GetSubsystem<UGoapWorldSubsystem>())
			{
				TargetActor = GoapWorldSubsystem->GetActorByKeyOrTag(Agent, TargetKey);
			}
		}
	}
	
	return TargetActor;
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::ExecuteMoveTo(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action)
{
	UBehaviorTreeComponent* OwnerComp = CachedBTComp.Get();
	if (!OwnerComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteMoveTo: Missing cached behavior tree component"));
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}

	const int32 InstanceIndex = OwnerComp->FindInstanceContainingNode(this);
	uint8* NodeMemory = InstanceIndex != INDEX_NONE ? OwnerComp->GetNodeMemory(this, InstanceIndex) : nullptr;
	return StartOrResumeMovement(*OwnerComp, GoapComp, Agent, Action, NodeMemory, TEXT("ExecuteMoveTo"), false);
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::ExecuteInteract(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action)
{
	AActor* TargetActor = FindTargetActor(GoapComp, Agent, Action->TargetActorKey);
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_ExecuteGoapAction: No actor found for interaction key '%s'"), *Action->TargetActorKey.ToString());
	    GoapComp->FailCurrentAction();
	    return EBTNodeResult::Failed;
	}

	if (UKismetSystemLibrary::DoesImplementInterface(TargetActor, UGOAPInteractable::StaticClass()))
	{
		IGOAPInteractable::Execute_Interact(TargetActor, Agent);
	}
	
	if (Action->AnimationMontage && Agent->GetMesh())
	{
		UAnimInstance* AnimInstance = Agent->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
	    	AnimInstance->Montage_Play(Action->AnimationMontage);
	    	return EBTNodeResult::InProgress;
		}

		UE_LOG(LogTemp, Warning, TEXT("BTTask_ExecuteGoapAction: Interact action '%s' has no animation instance; completing without montage"),
			*Action->ActionName.ToString());
	}
	
	GoapComp->CompleteCurrentAction();
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::ExecuteWait(UGoapComponent* GoapComp, ACharacter* Agent, const FGoapActionInstance& ActionInstance, uint8* NodeMemory)
{
	if (!GoapComp || !Agent || !ActionInstance.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteWait: Invalid input (GoapComp=%s Agent=%s ActionValid=%s)"), GoapComp ? TEXT("yes") : TEXT("no"), Agent ? TEXT("yes") : TEXT("no"), ActionInstance.IsValid() ? TEXT("yes") : TEXT("no"));
		if (GoapComp)
		{
			GoapComp->FailCurrentAction();
		}
		return EBTNodeResult::Failed;
	}
	
	UGoapActionDataAsset* Action = ActionInstance.SourceDataAsset;
    
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	if (Memory)
	{
		Memory->WaitTimeRemaining = ActionInstance.RuntimeDuration;  // Use runtime duration instead of template
		Memory->bIsWaiting = true;
		Memory->bWaitingForInteraction = (Action->WaitType == EGoapWaitType::Interaction || 
										  Action->WaitType == EGoapWaitType::TimeOrInteraction);
		Memory->bInteractionReceived = false;
        
		UE_LOG(LogTemp, Warning, TEXT("ExecuteWait: bWaitingForInteraction=%s, WaitType=%d, RuntimeDuration=%.2f"), 
			Memory->bWaitingForInteraction ? TEXT("TRUE") : TEXT("FALSE"),
			(int32)Action->WaitType,
			ActionInstance.RuntimeDuration);
		
		// Optionally play an animation during the wait
		if (Action->AnimationMontage)
		{
			USkeletalMeshComponent* Mesh = Agent->GetMesh();
			if (Mesh)
			{
				UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
				if (AnimInstance)
				{
					float MontageLength = AnimInstance->Montage_Play(Action->AnimationMontage);
					if (MontageLength > 0.0f)
					{
						UE_LOG(LogTemp, Log, TEXT("ExecuteWait: Playing animation '%s' during wait (MontageLength=%.2f, WaitDuration=%.2f)"), 
							*Action->AnimationMontage->GetName(), MontageLength, Action->Duration);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("ExecuteWait: Failed to play animation montage '%s'"), 
							*Action->AnimationMontage->GetName());
					}
				}
			}
		}
	}
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::ExecuteWaitForAnimation(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action)
{
	if (!GoapComp || !Agent || !Action)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteWaitForAnimation: Invalid input (GoapComp=%s Agent=%s Action=%s)"), GoapComp ? TEXT("yes") : TEXT("no"), Agent ? TEXT("yes") : TEXT("no"), Action ? TEXT("yes") : TEXT("no"));
		if (GoapComp)
		{
			GoapComp->FailCurrentAction();
		}
		return EBTNodeResult::Failed;
	}
	
	// Check if animation montage is specified
	if (!Action->AnimationMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteWaitForAnimation: No animation montage specified for action '%s'"), *Action->ActionName.ToString());
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}
	
	// Get the mesh and animation instance
	USkeletalMeshComponent* Mesh = Agent->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteWaitForAnimation: Agent has no mesh component"));
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}
	
	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteWaitForAnimation: Mesh has no animation instance"));
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}
	
	// Play the animation montage
	float MontageLength = AnimInstance->Montage_Play(Action->AnimationMontage);
	if (MontageLength <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteWaitForAnimation: Failed to play animation montage '%s'"), *Action->AnimationMontage->GetName());
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}
	
	// Return InProgress - TickTask will monitor when the animation completes
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::ExecuteInteractAndWaitForNotify(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory)
{
	if (!GoapComp || !Agent || !Action)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteInteractAndWaitForNotify: Invalid input (GoapComp=%s Agent=%s Action=%s)"), GoapComp ? TEXT("yes") : TEXT("no"), Agent ? TEXT("yes") : TEXT("no"), Action ? TEXT("yes") : TEXT("no"));
		if (GoapComp)
		{
			GoapComp->FailCurrentAction();
		}
		return EBTNodeResult::Failed;
	}

	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	if (Memory)
	{
		Memory->bWaitingForNotify = false;
		Memory->bInteractionReceived = false;
		Memory->bMovementCompleted = false;
		Memory->bMoveRequestActive = false;
		Memory->MovementRetryCount = 0;
	}
	
	// Find the target actor
	AActor* TargetActor = FindTargetActor(GoapComp, Agent, Action->TargetActorKey);
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteInteractAndWaitForNotify: No actor found for key '%s'"), *Action->TargetActorKey.ToString());
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}

	const auto StartInteractionAndWait = [&]() -> EBTNodeResult::Type
	{
		// Call the Interact function on the target if it implements the interface
		if (UKismetSystemLibrary::DoesImplementInterface(TargetActor, UGOAPInteractable::StaticClass()))
		{
			IGOAPInteractable::Execute_Interact(TargetActor, Agent);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ExecuteInteractAndWaitForNotify: Target actor '%s' does not implement Interractable interface"), *TargetActor->GetName());
		}

		// Play animation montage if specified
		if (Action->AnimationMontage && Agent->GetMesh())
		{
			UAnimInstance* AnimInstance = Agent->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(Action->AnimationMontage);
			}
		}

		if (Memory)
		{
			Memory->bWaitingForNotify = true;
			Memory->bInteractionReceived = false;
		}

		UE_LOG(LogTemp, Warning, TEXT("ExecuteInteractAndWaitForNotify: Waiting for notification on action '%s'"), *Action->ActionName.ToString());
		return EBTNodeResult::InProgress;
	};

	if (Action->bMoveToTargetBeforeInteractNotify)
	{
		UBehaviorTreeComponent* OwnerComp = CachedBTComp.Get();
		if (!OwnerComp)
		{
			UE_LOG(LogTemp, Error, TEXT("ExecuteInteractAndWaitForNotify: Missing cached behavior tree component before movement start"));
			GoapComp->FailCurrentAction();
			return EBTNodeResult::Failed;
		}

		return StartOrResumeMovement(*OwnerComp, GoapComp, Agent, Action, NodeMemory, TEXT("ExecuteInteractAndWaitForNotify"), false);
	}
	
	return StartInteractionAndWait();
}

EBTNodeResult::Type UBTTask_ExecuteGoapAction::ExecuteMoveToAndInteract(UGoapComponent* GoapComp, ACharacter* Agent, UGoapActionDataAsset* Action, uint8* NodeMemory)
{
	if (!GoapComp || !Agent || !Action)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteMoveToAndInteract: Invalid input (GoapComp=%s Agent=%s Action=%s)"), GoapComp ? TEXT("yes") : TEXT("no"), Agent ? TEXT("yes") : TEXT("no"), Action ? TEXT("yes") : TEXT("no"));
		if (GoapComp)
		{
			GoapComp->FailCurrentAction();
		}
		return EBTNodeResult::Failed;
	}
	
	// Initialize memory
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	if (Memory)
	{
		Memory->bMovementCompleted = false;
		Memory->bMoveRequestActive = false;
		Memory->MovementRetryCount = 0;
	}

	UBehaviorTreeComponent* OwnerComp = CachedBTComp.Get();
	if (!OwnerComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteMoveToAndInteract: Missing cached behavior tree component"));
		GoapComp->FailCurrentAction();
		return EBTNodeResult::Failed;
	}

	return StartOrResumeMovement(*OwnerComp, GoapComp, Agent, Action, NodeMemory, TEXT("ExecuteMoveToAndInteract"), false);
}

uint16 UBTTask_ExecuteGoapAction::GetInstanceMemorySize() const
{
	return sizeof(FBTExecuteGoapActionMemory);
}

void UBTTask_ExecuteGoapAction::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	// Early exit if components are no longer valid (task was aborted/finished)
	if (!CachedBTComp.IsValid() || !CachedController.IsValid())
	{
		UE_LOG(LogTemp, Verbose, TEXT("OnMoveCompleted: Task already finished, ignoring callback"));
		return;
	}

	if (CachedMoveRequestId.IsValid() && RequestID != CachedMoveRequestId)
	{
		UE_LOG(LogTemp, Verbose, TEXT("OnMoveCompleted: Ignoring move completion for a different active request"));
		return;
	}
	
	UBehaviorTreeComponent* OwnerComp = CachedBTComp.Get();
	ResetCachedMoveState();
	
	// Validate the behavior tree component is still active
	if (!OwnerComp || !OwnerComp->IsActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted: BT component is no longer active"));
		CachedBTComp.Reset();
		CachedController.Reset();
		return;
	}
	
	// Get the agent and GOAP component
	AAIController* AIController = OwnerComp->GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted: No AI controller found"));
		return;
	}
	
	ACharacter* Agent = Cast<ACharacter>(AIController->GetPawn());
	if (!Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted: No agent found"));
		return;
	}
	
	UGoapComponent* GoapComp = FAIComponentUtils::GetGoapComponent(Agent);
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted: No GOAP component found"));
		return;
	}
	
	// Get current action to check interaction type
	FGoapActionInstance CurrentAction = GoapComp->GetCurrentAction();
	if (!CurrentAction.IsValid() || !CurrentAction.SourceDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted: No valid current action"));
		return;
	}

	const int32 InstanceIndex = OwnerComp->FindInstanceContainingNode(this);
	uint8* NodeMemory = InstanceIndex != INDEX_NONE ? OwnerComp->GetNodeMemory(this, InstanceIndex) : nullptr;
	FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory);
	if (Memory)
	{
		Memory->bMoveRequestActive = false;
	}
	
	// Check if movement was successful
	bool bSuccess = (Result == EPathFollowingResult::Success);
	
	UE_LOG(LogTemp, Log, TEXT("OnMoveCompleted: Result=%d (%s)"), 
		(int32)Result, 
		bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
	
	if (bSuccess)
	{
		UGoapActionDataAsset* ActionTemplate = CurrentAction.SourceDataAsset;
		AActor* TargetActor = nullptr;
		float DistanceToTarget = 0.0f;
		if (!IsAgentAtActionDestination(GoapComp, Agent, ActionTemplate, TargetActor, DistanceToTarget))
		{
			const EBTNodeResult::Type RetryResult = HandleMovementNotAtDestination(*OwnerComp, GoapComp, Agent, ActionTemplate, NodeMemory, DistanceToTarget, TEXT("OnMoveCompleted"));
			if (RetryResult != EBTNodeResult::InProgress)
			{
				FinishLatentTask(*OwnerComp, RetryResult);
			}
			return;
		}

		const EBTNodeResult::Type CompletionResult = HandleMovementArrived(*OwnerComp, GoapComp, Agent, ActionTemplate, NodeMemory, TargetActor, TEXT("OnMoveCompleted"));
		if (CompletionResult != EBTNodeResult::InProgress)
		{
			FinishLatentTask(*OwnerComp, CompletionResult);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OnMoveCompleted: MoveTo failed with result: %d"), (int32)Result);
		GoapComp->FailCurrentAction();
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}
}

void UBTTask_ExecuteGoapAction::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	// Ensure delegate is unbound when task finishes for any reason
	if (FBTExecuteGoapActionMemory* Memory = GetTaskMemory(NodeMemory))
	{
		Memory->bMoveRequestActive = false;
	}
	ResetCachedMoveState();
	
	CachedBTComp.Reset();
	
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}






