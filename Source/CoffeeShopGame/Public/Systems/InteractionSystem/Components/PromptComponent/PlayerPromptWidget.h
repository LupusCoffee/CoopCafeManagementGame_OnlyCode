#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerPromptWidget.generated.h"

class UPromptWidgetBox;
enum class EAction : uint8;

UCLASS()
class COFFEESHOPGAME_API UPlayerPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPrompts(const TArray<EAction>& InActions);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddPrompts(const TArray<EAction>& InActions);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemovePrompts(const TArray<EAction>& InActions);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClearPrompts();
};
