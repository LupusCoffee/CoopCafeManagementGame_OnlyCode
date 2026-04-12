#include "Systems/ActionStack/ActionStack.h"
#include "Systems/ActionStack/Actions/NewGameAction.h"


//Setup
void UActionStack::SetupAllActions(TObjectPtr<UActionStackContext> InActionStackContext)
{
	ActionStackContext = InActionStackContext;
	
	//find all classes that inherit from base action
	TArray<TSubclassOf<UBaseAction>> FoundActionClasses;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		if (Class->IsChildOf(UBaseAction::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract))
		{
			FoundActionClasses.Add(Class);
		}
	}

	//create action objects out of all the classes found + cache
	for (auto ActionClass : FoundActionClasses)
	{
		UBaseAction* Action = NewObject<UBaseAction>(this, ActionClass);
		AvailableActions.FindOrAdd(ActionClass) = Action;
	}
}

void UActionStack::SetupSpecificActions(TObjectPtr<UActionStackContext> InActionStackContext, TArray<UBaseAction*> ActionsToUse)
{
	ActionStackContext = InActionStackContext;
	
	for (auto Action : ActionsToUse)
	{
		AvailableActions.FindOrAdd(Action->GetClass()) = Action;
	}
}


//Add, Tick
bool UActionStack::PushAction(UClass* ActionClass)
{
	// get state & null checks
	if (!ActionClass) return false;
	
	TObjectPtr<UBaseAction>* FoundAction = AvailableActions.Find(ActionClass);
	if (!FoundAction) return false;

	UBaseAction* Action = *FoundAction;
	if (!IsValid(Action)) return false;

	
	// check if the action is anywhere else on the stack -> if it is, remove all those instances
	// --> note: this might actually be a bad idea. idk
	for (int32 i = Stack.Num()-1; i >= 0; --i)
	{
		if (Stack[i] == Action) Stack.RemoveAt(i);
	}

	
	// enter
	Stack.Insert(Action, 0);
	CurrentAction = Action;
	CurrentAction->OnBegin(true);
	
	return true;
}

void UActionStack::Tick(float DeltaTime)
{
	if (Stack.IsEmpty()) return;
	if (!CurrentAction) return;

	CurrentAction->OnTick(DeltaTime);

	if (CurrentAction->IsDone())
	{
		UBaseAction* ActionToRemove = CurrentAction;
		ActionToRemove->OnEnd();
		Stack.Remove(ActionToRemove);
		if (CurrentAction == ActionToRemove)
		{
			if (!Stack.IsEmpty())
			{
				//coming back to previous action in the stack -> not first time anymore
				CurrentAction = Stack[0];
				CurrentAction->OnBegin(false);
			}
			else CurrentAction = nullptr;
		}
	}
}


//Getters
UActionStackContext* UActionStack::GetActionStackContext()
{
	return ActionStackContext;
}

bool UActionStack::IsEmpty()
{
	return Stack.IsEmpty();
}
