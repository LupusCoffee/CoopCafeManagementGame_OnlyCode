// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAP_Component.h"
#include "GOAP_Action.h"
#include "GOAP_Goal.h"
#include "GOAP_Planner.h"
#include "AIController.h"
#include "Actions/GOAP_DrinkCoffeeAction.h"
#include "Actions/GOAP_FIndStoreAction.h"
#include "Actions/GOAP_QueueAction.h"
#include "Actions/GOAP_WaitForOrderAction.h"
#include "Actions/Idle.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Goals/DrinkCoffee.h"
#include "Goals/IdleGoal.h"


void UGOAP_Component::BeginPlay()
{	
	Super::BeginPlay();
}

// Sets default values for this component's properties
UGOAP_Component::UGOAP_Component()
{
	PrimaryComponentTick.bCanEverTick = true;

	Planner = CreateDefaultSubobject<UGOAP_Planner>(TEXT("Planner"));

	// Create actions
	UGOAP_DrinkCoffeeAction* DrinkCoffeeAction = CreateDefaultSubobject<UGOAP_DrinkCoffeeAction>(TEXT("DrinkCoffeeAction"));
	UIdle* IdleAction = CreateDefaultSubobject<UIdle>(TEXT("IdleAction"));
	UGOAP_FIndStoreAction* FindStoreAction = CreateDefaultSubobject<UGOAP_FIndStoreAction>(TEXT("FindStoreAction"));
	UGOAP_QueueAction* QueueAction = CreateDefaultSubobject<UGOAP_QueueAction>(TEXT("QueueAction"));
	UGOAP_WaitForOrderAction* WaitForOrderAction = CreateDefaultSubobject<UGOAP_WaitForOrderAction>(TEXT("WaitForOrderAction"));

	AvailableActions = { DrinkCoffeeAction, IdleAction, FindStoreAction, QueueAction, WaitForOrderAction };

	// Create goals
	UDrinkCoffee* DrinkCoffeeGoal = CreateDefaultSubobject<UDrinkCoffee>(TEXT("DrinkCoffeeGoal"));
	UIdleGoal* IdleGoal = CreateDefaultSubobject<UIdleGoal>(TEXT("IdleGoal"));
	AvailableGoals = { DrinkCoffeeGoal, IdleGoal };
}

void UGOAP_Component::EvaluateGoals()
{
	if (!Planner || AvailableActions.Num() == 0 || AvailableGoals.Num() == 0) return;

	CurrentPlan.Empty();
	
	// Pick highest-priority valid goal
	UGOAP_Goal* BestGoal = nullptr;
	float BestPriority = -1.f;

	for (UGOAP_Goal* Goal : AvailableGoals)
	{
		if (!Goal || Goal->IsValid(CurrentWorldState)) continue;
		if (Goal->Priority > BestPriority)
		{
			BestGoal = Goal;
			BestPriority = Goal->Priority;
		}
	}

	if (!BestGoal) return;

	CurrentPlan = Planner->Plan(AvailableActions, CurrentWorldState, BestGoal);
	CurrentActionIndex = 0;
	StartNextAction();
}

void UGOAP_Component::AdvanceToNextAction()
{
	CurrentActionIndex++;
	if (CurrentActionIndex < CurrentPlan.Num())
	{
		StartNextAction();
	}
	else
	{
		EvaluateGoals();
	}
}

void UGOAP_Component::StartNextAction()
{
	if (CurrentActionIndex >= CurrentPlan.Num()) return;

	UGOAP_Action* Action = CurrentPlan[CurrentActionIndex];
	if (!Action) return;

	if (AAIController* AI = Cast<AAIController>(GetOwner()))
	{
		if (UBlackboardComponent* BB = AI->GetBlackboardComponent())
		{
			BB->SetValueAsEnum(FName("CurrentAction"), static_cast<uint8>(Action->ActionEnum));
		}
	}
}
