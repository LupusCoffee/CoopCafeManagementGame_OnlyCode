#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardUI.generated.h"
class UTurnBasedSession;

UCLASS()
class COFFEESHOPGAME_API UCardUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetClickable(bool Value);
	
protected:
	UPROPERTY(BlueprintReadWrite)
	bool bClickable = false;

	UPROPERTY(BlueprintReadWrite)
	UTurnBasedSession* Session = nullptr;
};
