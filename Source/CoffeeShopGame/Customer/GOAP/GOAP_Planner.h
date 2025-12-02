// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GOAP_Planner.generated.h"

class UGOAP_Goal;
struct FWorldState;
class UGOAP_Action;
/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UGOAP_Planner : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<UGOAP_Action*> Plan(const TArray<UGOAP_Action*>& AvailableActions, const FWorldState& CurrentState,const UGOAP_Goal* GoalState);

private:
	UFUNCTION()
	static FWorldState ApplyEffects(UGOAP_Action* Action, const FWorldState& State);
	
};
