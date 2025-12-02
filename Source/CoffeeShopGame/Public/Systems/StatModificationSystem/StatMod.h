#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Enums/ModificationType.h"
#include "UObject/Object.h"
#include "StatMod.generated.h"

USTRUCT(BlueprintType)
struct FStatModEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stat.PropertyStat"))
	FGameplayTag PropertyStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EModificationType ModificationType = EModificationType::Multiplication;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude = 0.0f;

	FStatModEntry() = default;
	FStatModEntry(FGameplayTag InPropertyStat, EModificationType InModificationType, float InMagnitude)
	{
		PropertyStat = InPropertyStat;
		ModificationType = InModificationType;
		Magnitude = InMagnitude;
	}

	bool operator==(const FStatModEntry& Other) const
	{
		if (Other.PropertyStat == PropertyStat && Other.ModificationType == ModificationType && Other.Magnitude == Magnitude) return true;
		else return false;
	}
	bool operator!=(const FStatModEntry& Other) const { return !(*this == Other); }
};


UCLASS(Blueprintable, BlueprintType)
class COFFEESHOPGAME_API UStatMod : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FStatModEntry> StatModEntries;
};