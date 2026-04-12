#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ActionPromptSubsystem.generated.h"

class UPlayerPromptWidget;
class UPromptWidgetBox;
struct FPrompt;
enum class EAction : uint8;
class UActionPromptsData;

UCLASS()
class COFFEESHOPGAME_API UActionPromptSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitSubsystem(UActionPromptsData* InActionPromptsData, TSubclassOf<UPromptWidgetBox> InPromptWidgetBox, 
		TSubclassOf<UPlayerPromptWidget> InPlayerPromptWidgetClass);
	
	UFUNCTION(BlueprintCallable)
	UActionPromptsData* GetActionPromptsData();
	
	UFUNCTION(BlueprintCallable)
	FPrompt GetPrompt(EAction Action);
	
	UFUNCTION(BlueprintCallable)
	UPromptWidgetBox* CreatePromptWidgetBox();
	
	UFUNCTION(BlueprintCallable)
	UPlayerPromptWidget* CreatePlayerPromptWidget(APlayerController* OwningPlayerController);
	
protected:
	UPROPERTY()
	UActionPromptsData* ActionPromptsData = nullptr;
	
	UPROPERTY()
	TSubclassOf<UPromptWidgetBox> PromptWidgetBoxClass = nullptr;
	
	UPROPERTY()
	TSubclassOf<UPlayerPromptWidget> PlayerPromptWidgetClass = nullptr;
};
