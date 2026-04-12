// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTDecorator_HasGoapAction.h"

#include "AIController.h"
#include "GOAPSystem/AI/Utils/AIComponentUtils.h"

UBTDecorator_HasGoapAction::UBTDecorator_HasGoapAction()
{
	NodeName = "Has GOAP Action";
}

bool UBTDecorator_HasGoapAction::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;
	
	AActor* Agent = AIController->GetPawn();
	if (!Agent) return false;
	
	UGoapComponent* GoapComp = FAIComponentUtils::GetGoapComponent(Agent);
	if (!GoapComp) return false;
	
	return GoapComp->HasActiveAction();
}
