#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CardData.generated.h"

UCLASS()
class COFFEESHOPGAME_API UCardData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FString CardName;
	
	UPROPERTY(EditDefaultsOnly)
	FString ImmediateDescription;

	UPROPERTY(EditDefaultsOnly)
	FString StackDescription;

	UPROPERTY(EditDefaultsOnly)
	int Points;
};
