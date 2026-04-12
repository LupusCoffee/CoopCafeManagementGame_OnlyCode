#pragma once

#include "CoreMinimal.h"
#include "ActionStack.h"
#include "UObject/Object.h"
#include "BaseAction.generated.h"
class UActionStack;

UCLASS(Abstract, Blueprintable, BlueprintType)
class COFFEESHOPGAME_API UBaseAction : public UObject
{
	GENERATED_BODY()

public:
	//virtual methods
	UFUNCTION(BlueprintNativeEvent)
	void OnBegin(bool bFirstTime);
	virtual void OnBegin_Implementation(bool bFirstTime);

	UFUNCTION(BlueprintNativeEvent)
	void OnTick(float DeltaTime);
	virtual void OnTick_Implementation(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnd();
	virtual void OnEnd_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	bool IsDone() const;
	virtual bool IsDone_Implementation() const;

	//getters
	template<typename T>
	T* GetActionStackContext() const;
	
protected:
	UPROPERTY()
	UActionStack* Stack;	//need to set the stack
};


//template definitions --> gotta be visible to everything
template <typename T>
T* UBaseAction::GetActionStackContext() const
{
	if (!Stack)
	{
		UE_LOG(LogTemp, Error, TEXT("TRYING TO ACCESS CONTEXT OF NULL ACTION STACK. (%s:%d)"), TEXT(__FILE__), __LINE__);
		ensureMsgf(false, TEXT("TRYING TO ACCESS CONTEXT OF NULL ACTION STACK. Ptr was null"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,TEXT("TRYING TO ACCESS CONTEXT OF NULL ACTION STACK."));
		return nullptr;
	}
	return Cast<T>(Stack->GetActionStackContext());
}