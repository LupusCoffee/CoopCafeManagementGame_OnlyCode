// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GOAPSystem/AI/Components/ActivatableComponent.h"
#include "GOAPSystem/AI/Components/AIDebugComponent.h"
#include "GOAPSystem/GOAP/Components/GoapComponent.h"

/**
 * Utility class for common AI component operations to reduce code duplication
 */
class GOAPSYSTEM_API FAIComponentUtils
{
public:
	// Component retrieval helpers
	static UActivatableComponent* GetActivatableComponent(AActor* Actor);
	static UGoapComponent* GetGoapComponent(AActor* Actor);
	static UAIDebugComponent* GetDebugComponent(AActor* Actor);
	static AAIController* GetAIController(AActor* Actor);
	static UBrainComponent* GetBrainComponent(AActor* Actor);
	
	// Combined component retrieval for performance
	struct FAIComponents
	{
		UActivatableComponent* ActivatableComp = nullptr;
		UGoapComponent* GoapComp = nullptr;
		UAIDebugComponent* DebugComp = nullptr;
		AAIController* Controller = nullptr;
		UBrainComponent* Brain = nullptr;
		
		bool IsValid() const { return ActivatableComp != nullptr || GoapComp != nullptr; }
	};
	
	static FAIComponents GetAllAIComponents(AActor* Actor);
	
	// AI state control helpers
	static void StopAIMovement(AActor* Actor);
	static void StopAILogic(AActor* Actor, const FString& Reason = TEXT("Stopped"));
	static void PauseAILogic(AActor* Actor, const FString& Reason = TEXT("Paused"));
	static void ResumeAILogic(AActor* Actor, const FString& Reason = TEXT("Resumed"));
	static void RestartAILogic(AActor* Actor);
	
	// GOAP-specific helpers
	static void RequestGoapReplan(AActor* Actor);
	static void ResetGoapExecution(AActor* Actor);
};

