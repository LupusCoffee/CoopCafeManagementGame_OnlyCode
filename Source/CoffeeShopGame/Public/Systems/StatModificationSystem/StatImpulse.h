#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Enums/ModificationType.h"
#include "UObject/Object.h"
#include "StatImpulse.generated.h"

USTRUCT(BlueprintType)
struct FStatImpulseEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stat.MeterStat"))
	FGameplayTag MeterStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EModificationType ModificationType = EModificationType::Multiplication;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude = 0.0f;

	FStatImpulseEntry() = default;
	FStatImpulseEntry(FGameplayTag InMeterStat, EModificationType InModificationType, float InMagnitude)
	{
		MeterStat = InMeterStat;
		ModificationType = InModificationType;
		Magnitude = InMagnitude;
	}

	bool operator==(const FStatImpulseEntry& Other) const
	{
		if (Other.MeterStat == MeterStat && Other.ModificationType == ModificationType && Other.Magnitude == Magnitude) return true;
		else return false;
	}
	bool operator!=(const FStatImpulseEntry& Other) const { return !(*this == Other); }
};

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOPGAME_API UStatImpulse : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FStatImpulseEntry> StatImpulseEntries;
};
