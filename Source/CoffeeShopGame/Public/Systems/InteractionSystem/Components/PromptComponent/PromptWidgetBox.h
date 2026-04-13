#pragma once

#include "CoreMinimal.h"
#include "ActionEnum.h"
#include "Blueprint/UserWidget.h"
#include "PromptWidgetBox.generated.h"
class UActionPromptsData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateDisplayedActionPrompts, const TArray<EAction>&, InActions);

UCLASS()
class COFFEESHOPGAME_API UPromptWidgetBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPrompts(const TArray<EAction>& InActions);
	
	UFUNCTION(BlueprintCallable)
	void AddPrompts(const TArray<EAction>& InActions);
	
	UFUNCTION(BlueprintCallable)
	void RemovePrompts(const TArray<EAction>& InActions);
	
	UFUNCTION(BlueprintCallable)
	void ClearPrompts();
	
	UPROPERTY(BlueprintAssignable)
	FOnUpdateDisplayedActionPrompts OnUpdateDisplayedActionPrompts;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<EAction> ActionsDisplayed;
};
