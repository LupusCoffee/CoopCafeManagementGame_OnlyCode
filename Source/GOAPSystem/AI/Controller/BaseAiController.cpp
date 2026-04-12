// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAiController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GOAPSystem/Managers/AIManager.h"


// Sets default values
ABaseAiController::ABaseAiController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseAiController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitializeAI();
}

void ABaseAiController::OnUnPossess()
{
	CleanupAI();
	ResetMovePriority();
	Super::OnUnPossess();
}

EAIManagerState ABaseAiController::GetAIState() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return EAIManagerState::Disabled;
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return EAIManagerState::Disabled;
	}
	
	UAIManager* AIManager = World->GetSubsystem<UAIManager>();
	if (!AIManager)
	{
		return EAIManagerState::Normal;  // Default if no manager
	}
	
	FAIAgentInfo AgentInfo;
	if (AIManager->GetAIInfo(ControlledPawn, AgentInfo))
	{
		return AgentInfo.CurrentState;
	}
	
	return EAIManagerState::Normal;  // Default state
}

FPathFollowingRequestResult ABaseAiController::MoveToActor(AActor* TargetActor, float AcceptanceRadius, EMovePriority MovePriority)
{
	if (MovePriority == EMovePriority::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to move with Idle priority. Ignoring MoveTo command."));
		FPathFollowingRequestResult Result;
		Result.Code = EPathFollowingRequestResult::Failed;
		return Result;
	}
	
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TargetActor passed to MoveToActor. Ignoring MoveTo command."));
		FPathFollowingRequestResult Result;
		Result.Code = EPathFollowingRequestResult::Failed;
		return Result;
	}
	
	FAIMoveRequest MoveRequest(TargetActor);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	return RequestMoveWithPriority(MoveRequest, MovePriority);
}

FPathFollowingRequestResult ABaseAiController::MoveToPosition(FVector TargetLocation, float AcceptanceRadius, EMovePriority MovePriority)
{
	if (MovePriority == EMovePriority::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to move with Idle priority. Ignoring MoveTo command."));
		FPathFollowingRequestResult Result;
		Result.Code = EPathFollowingRequestResult::Failed;
		return Result;
	}
	
	FAIMoveRequest MoveRequest(TargetLocation);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	return RequestMoveWithPriority(MoveRequest, MovePriority);
}

FPathFollowingRequestResult ABaseAiController::RequestMoveWithPriority(const FAIMoveRequest& MoveRequest, EMovePriority MovePriority)
{
	FPathFollowingRequestResult FailureResult;
	FailureResult.Code = EPathFollowingRequestResult::Failed;
	
	if (MovePriority == EMovePriority::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to move with Idle priority. Ignoring MoveTo command."));
		return FailureResult;
	}
	
	if (CurrentMovePriority != EMovePriority::Idle && MovePriority < CurrentMovePriority)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Ignoring MoveTo request: priority %d < current %d"), static_cast<int32>(MovePriority), static_cast<int32>(CurrentMovePriority));
		return FailureResult;
	}
	
	FPathFollowingRequestResult Result = MoveTo(MoveRequest);
	if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
	{
		CurrentMovePriority = MovePriority;
		ActiveMoveRequestId = Result.MoveId;
	}
	else if (Result.Code == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		ResetMovePriority();
	}
	
	return Result;
}

void ABaseAiController::InitializeAI()
{
	if (BlackboardData)
	{
		UBlackboardComponent* BlackboardComp;
		UseBlackboard(BlackboardData, BlackboardComp);
	}
    
	// Run behavior tree
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ABaseAiController::CleanupAI()
{
	if (UBrainComponent* Brain = GetBrainComponent())
	{
		Brain->StopLogic(TEXT("Unpossessed"));
	}
	ResetMovePriority();
}

void ABaseAiController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (ActiveMoveRequestId.IsValid() && RequestID != ActiveMoveRequestId)
	{
		return;
	}

	ResetMovePriority();
}

void ABaseAiController::ResetMovePriority()
{
	CurrentMovePriority = EMovePriority::Idle;
	ActiveMoveRequestId = FAIRequestID::InvalidRequest;
}
