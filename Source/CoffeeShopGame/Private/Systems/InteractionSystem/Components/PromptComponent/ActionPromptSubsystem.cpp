#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionPromptSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionPromptsData.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PlayerPromptWidget.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"

void UActionPromptSubsystem::InitSubsystem(UActionPromptsData* InActionPromptsData, TSubclassOf<UPromptWidgetBox> InPromptWidgetBox,
                                           TSubclassOf<UPlayerPromptWidget> InPlayerPromptWidgetClass)
{
	ActionPromptsData = InActionPromptsData;
	PromptWidgetBoxClass = InPromptWidgetBox;
	PlayerPromptWidgetClass = InPlayerPromptWidgetClass;
}

UActionPromptsData* UActionPromptSubsystem::GetActionPromptsData()
{
	return ActionPromptsData;
}

FPrompt UActionPromptSubsystem::GetPrompt(EAction Action)
{
	for (auto ActionPromptDataEntry : ActionPromptsData->GetEntries())
	{
		if (ActionPromptDataEntry.Action == Action) return ActionPromptDataEntry.Prompt;
	}
	return FPrompt();
}

UPromptWidgetBox* UActionPromptSubsystem::CreatePromptWidgetBox()
{
	if (!PromptWidgetBoxClass) return nullptr;
	return CreateWidget<UPromptWidgetBox>(GetWorld(), PromptWidgetBoxClass);
}

UPlayerPromptWidget* UActionPromptSubsystem::CreatePlayerPromptWidget(APlayerController* OwningPlayerController)
{
	if (!PlayerPromptWidgetClass) return nullptr;
	return CreateWidget<UPlayerPromptWidget>(OwningPlayerController, PlayerPromptWidgetClass);
}
