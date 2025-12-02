// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Customer/Enums/GOAP_Enums.h"

#include "Components/ActorComponent.h"
#include "GOAP_Component.generated.h"

class UGOAP_Goal;
class UGOAP_Planner;
class UGOAP_Action;

USTRUCT(BlueprintType)
struct FWorldState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thirstiness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasCoffee = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAtStore = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasOrdered = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UGOAP_Component : public UActorComponent
{
public:
	virtual void BeginPlay() override;

private:
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGOAP_Component();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FWorldState CurrentWorldState;

	UPROPERTY()
	UGOAP_Planner* Planner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UGOAP_Action*> AvailableActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UGOAP_Goal*> AvailableGoals;

	UPROPERTY(BlueprintReadOnly, Category = "GOAP")
	TArray<UGOAP_Action*> CurrentPlan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGOAPActions CurrentAction = EGOAPActions::None;

	UPROPERTY()
	int32 CurrentActionIndex = 0;

public:
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void EvaluateGoals();

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void AdvanceToNextAction();
private:
	void StartNextAction();	
};
