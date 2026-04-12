// Fill out your copyright notice in the Description page of Project Settings.

#include "AIComponentUtils.h"

UActivatableComponent* FAIComponentUtils::GetActivatableComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UActivatableComponent>() : nullptr;
}

UGoapComponent* FAIComponentUtils::GetGoapComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGoapComponent>() : nullptr;
}

UAIDebugComponent* FAIComponentUtils::GetDebugComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UAIDebugComponent>() : nullptr;
}

AAIController* FAIComponentUtils::GetAIController(AActor* Actor)
{
	if (!Actor)
		return nullptr;
	
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Cast<AAIController>(Pawn->GetController());
	}
	
	return Cast<AAIController>(Actor);
}

UBrainComponent* FAIComponentUtils::GetBrainComponent(AActor* Actor)
{
	if (AAIController* Controller = GetAIController(Actor))
	{
		return Controller->GetBrainComponent();
	}
	return nullptr;
}

FAIComponentUtils::FAIComponents FAIComponentUtils::GetAllAIComponents(AActor* Actor)
{
	FAIComponents Components;
	
	if (!Actor)
		return Components;
	
	Components.ActivatableComp = Actor->FindComponentByClass<UActivatableComponent>();
	Components.GoapComp = Actor->FindComponentByClass<UGoapComponent>();
	Components.DebugComp = Actor->FindComponentByClass<UAIDebugComponent>();
	Components.Controller = GetAIController(Actor);
	Components.Brain = Components.Controller ? Components.Controller->GetBrainComponent() : nullptr;
	
	return Components;
}

void FAIComponentUtils::StopAIMovement(AActor* Actor)
{
	if (AAIController* Controller = GetAIController(Actor))
	{
		Controller->StopMovement();
	}
}

void FAIComponentUtils::StopAILogic(AActor* Actor, const FString& Reason)
{
	if (UBrainComponent* Brain = GetBrainComponent(Actor))
	{
		Brain->StopLogic(Reason);
	}
}

void FAIComponentUtils::PauseAILogic(AActor* Actor, const FString& Reason)
{
	if (UBrainComponent* Brain = GetBrainComponent(Actor))
	{
		Brain->PauseLogic(Reason);
	}
}

void FAIComponentUtils::ResumeAILogic(AActor* Actor, const FString& Reason)
{
	if (UBrainComponent* Brain = GetBrainComponent(Actor))
	{
		Brain->ResumeLogic(Reason);
	}
}

void FAIComponentUtils::RestartAILogic(AActor* Actor)
{
	if (UBrainComponent* Brain = GetBrainComponent(Actor))
	{
		Brain->RestartLogic();
	}
}

void FAIComponentUtils::RequestGoapReplan(AActor* Actor)
{
	if (UGoapComponent* GoapComp = GetGoapComponent(Actor))
	{
		GoapComp->RequestReplan();
	}
}

void FAIComponentUtils::ResetGoapExecution(AActor* Actor)
{
	if (UGoapComponent* GoapComp = GetGoapComponent(Actor))
	{
		GoapComp->ResetExecutionState();
	}
}

