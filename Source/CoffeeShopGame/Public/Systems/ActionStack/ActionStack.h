#pragma once

#include "CoreMinimal.h"
#include "ActionStackContext.h"
#include "UObject/Object.h"
#include "ActionStack.generated.h"
class UBaseAction;

UCLASS(BlueprintType)
class COFFEESHOPGAME_API UActionStack : public UObject
{
	GENERATED_BODY()
	

public:
	//Action Setting
	void SetupAllActions(TObjectPtr<UActionStackContext> InActionStackContext);
	void SetupSpecificActions(TObjectPtr<UActionStackContext> InActionStackContext, TArray<UBaseAction*> ActionsToUse);

	UFUNCTION(BlueprintCallable)
	bool PushAction(UClass* ActionClass);
	void Tick(float DeltaTime);

	//Getters
	UFUNCTION()
	UActionStackContext* GetActionStackContext();
	
	UFUNCTION(BlueprintCallable)
	bool IsEmpty();
	
private:	
	//Variables - Setup
	UPROPERTY()
	TMap<TSubclassOf<UBaseAction>, TObjectPtr<UBaseAction>> AvailableActions;

	UPROPERTY()
	TObjectPtr<UActionStackContext> ActionStackContext;
	
	//Variables - Other
	UPROPERTY()
	UBaseAction* CurrentAction;

	UPROPERTY()
	TArray<UBaseAction*> Stack;
};
