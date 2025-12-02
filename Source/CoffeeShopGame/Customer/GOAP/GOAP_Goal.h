// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAP_Component.h"
#include "UObject/Object.h"
#include "GOAP_Goal.generated.h"

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UGOAP_Goal : public UObject
{
	GENERATED_BODY()

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GoalName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Priority = 1.0f; //More = higher prio between 0.0f - 1.0f

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldState DesiredState;

	virtual bool IsValid(FWorldState CurrentState) const
	{
		return true;
	} 
};
