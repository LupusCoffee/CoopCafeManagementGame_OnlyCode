#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"

void UPromptWidgetBox::SetPrompts(const TArray<EAction>& InActions)
{
	ActionsDisplayed = InActions;
	
	OnUpdateDisplayedActionPrompts.Broadcast(ActionsDisplayed);
}

void UPromptWidgetBox::AddPrompts(const TArray<EAction>& InActions)
{
	for (auto Element : InActions)
	{
		ActionsDisplayed.Add(Element);
	}
	
	OnUpdateDisplayedActionPrompts.Broadcast(ActionsDisplayed);
}

void UPromptWidgetBox::RemovePrompts(const TArray<EAction>& InActions)
{
	for (auto Element : InActions)
	{
		ActionsDisplayed.Remove(Element);
	}
	
	OnUpdateDisplayedActionPrompts.Broadcast(ActionsDisplayed);
}

void UPromptWidgetBox::ClearPrompts()
{
	ActionsDisplayed.Empty();
	
	OnUpdateDisplayedActionPrompts.Broadcast(ActionsDisplayed);
}
