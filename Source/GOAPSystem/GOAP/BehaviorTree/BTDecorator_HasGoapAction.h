// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "../Components/GoapComponent.h"
#include "BTDecorator_HasGoapAction.generated.h"

/** Checks if agent has an active GOAP action */
UCLASS()
class GOAPSYSTEM_API UBTDecorator_HasGoapAction : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HasGoapAction();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
