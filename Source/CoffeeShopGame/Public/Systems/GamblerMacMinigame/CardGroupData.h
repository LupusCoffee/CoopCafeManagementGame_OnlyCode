#pragma once

#include "CoreMinimal.h"
#include "CardData.h"
#include "Engine/DataAsset.h"
#include "CardGroupData.generated.h"

USTRUCT(BlueprintType)
struct FCardEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardData* CardData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Amount;
};

UCLASS()
class COFFEESHOPGAME_API UCardGroupData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FCardEntry> Cards;
};
