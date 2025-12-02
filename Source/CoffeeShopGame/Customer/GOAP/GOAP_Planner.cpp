// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAP_Planner.h"
#include "GOAP_Action.h"
#include "GOAP_Goal.h"
#include "GOAP_Component.h"

TArray<UGOAP_Action*> UGOAP_Planner::Plan(const TArray<UGOAP_Action*>& AvailableActions,
	const FWorldState& CurrentState, const UGOAP_Goal* GoalState)
{

	struct FNode
	{
		FWorldState State;
		UGOAP_Action* Action;
		FNode* Parent;
		float Cost;
	};

	TArray<FNode*> OpenList;
	TArray<FNode*> ClosedList;

	FNode* Start = new FNode{ CurrentState, nullptr, nullptr, 0 };
	OpenList.Add(Start);

	FNode* GoalNode = nullptr;

	while (OpenList.Num() > 0)
	{
		OpenList.Sort([](const FNode& A, const FNode& B) { return A.Cost < B.Cost; });
		auto* Current = OpenList[0];
		OpenList.RemoveAt(0);

		if (GoalState->IsValid(Current->State))
		{
			GoalNode = Current;
			break;
		}

		for (auto* Action : AvailableActions)
		{
			if (!Action) continue;
			if (Action->CheckProceduralPrecondition(Current->State))
			{
				auto NewState = ApplyEffects(Action, Current->State);
				auto* NewNode = new FNode{ NewState, Action, Current, Current->Cost + 1.0f };
				OpenList.Add(NewNode);
			}
		}

		ClosedList.Add(Current);
	}
	
	TArray<UGOAP_Action*> Plan;
	if (GoalNode)
	{
		auto* Node = GoalNode;
		while (Node->Parent != nullptr)
		{
			Plan.Insert(Node->Action, 0);
			Node = Node->Parent;
		}
	}

	for (auto* Node : OpenList) delete Node;
	for (auto* Node : ClosedList) delete Node;

	return Plan;
}

FWorldState UGOAP_Planner::ApplyEffects(UGOAP_Action* Action, const FWorldState& State)
{
	FWorldState NewState = State;

	Action->ApplyMutatedEffect(NewState);

	return NewState;
}
