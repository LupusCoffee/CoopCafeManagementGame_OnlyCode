// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GOAPSystem/Managers/Structs/ManagerHelperStructs.h"
#include "Navigation/PathFollowingComponent.h"
#include "GOAPSystem/AI/Controller/MovePriority.h"
#include "BaseAiController.generated.h"

UCLASS()
class GOAPSYSTEM_API ABaseAiController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseAiController();
	
	//---------------------CONFIGURATION---------------------//
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBlackboardData* BlackboardData;
	
	//---------------------LIFECYCLE---------------------//
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
	//---------------------STATE---------------------//
	UFUNCTION(BlueprintPure, Category = "AI")
	EAIManagerState GetAIState() const;
	
	// Blueprint-exposed wrappers removed to avoid unsupported return type in UHT; call these from C++.
	FPathFollowingRequestResult MoveToActor(AActor* TargetActor, float AcceptanceRadius = 5.0f, EMovePriority MovePriority = EMovePriority::Normal);
	FPathFollowingRequestResult MoveToPosition(FVector TargetLocation, float AcceptanceRadius = 5.0f, EMovePriority MovePriority = EMovePriority::Normal);

	FPathFollowingRequestResult RequestMoveWithPriority(const FAIMoveRequest& MoveRequest, EMovePriority MovePriority);
	EMovePriority GetCurrentMovePriority() const { return CurrentMovePriority; }
	void ResetMovePriority();

protected:
	//---------------------AI SYSTEMS---------------------//
	/** Initialize */
	virtual void InitializeAI();
    
	/** Cleanup */
	virtual void CleanupAI();

	//-----------------------MOVEMENT---------------------//
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
private:
	UPROPERTY()
	EMovePriority CurrentMovePriority = EMovePriority::Idle;

	FAIRequestID ActiveMoveRequestId = FAIRequestID::InvalidRequest;
};
