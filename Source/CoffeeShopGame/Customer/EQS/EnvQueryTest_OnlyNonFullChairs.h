// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_OnlyNonFullChairs.generated.h"

/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UEnvQueryTest_OnlyNonFullChairs : public UEnvQueryTest
{
public:
	UEnvQueryTest_OnlyNonFullChairs(const FObjectInitializer& ObjectInitializer);
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

private:
	GENERATED_BODY()
};
