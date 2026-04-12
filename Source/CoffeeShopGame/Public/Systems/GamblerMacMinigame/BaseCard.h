#pragma once

#include "CoreMinimal.h"
#include "CardData.h"
#include "UObject/Object.h"
#include "BaseCard.generated.h"

UCLASS(BlueprintType)
class COFFEESHOPGAME_API UBaseCard : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetupData(UCardData* CardData);

	UFUNCTION(BlueprintCallable)
	void ImmediateEffect();

	UFUNCTION(BlueprintCallable)
	void StackEffect();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CardName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ImmediateDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString StackDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Points;
};
