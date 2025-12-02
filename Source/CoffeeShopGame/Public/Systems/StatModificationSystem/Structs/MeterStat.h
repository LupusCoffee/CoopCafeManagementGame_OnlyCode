#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StatOrValue.h"
#include <limits>

#include "StatOrValueWithModificationType.h"
#include "MeterStat.generated.h"

USTRUCT(BlueprintType)
struct FMeterStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stat.MeterStat"))
	FGameplayTag MeterStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attached Property Stats", meta = (Categories = "Stat.PropertyStat"))
	FStatOrValue Max = FStatOrValue(std::numeric_limits<float>::max());
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attached Property Stats", meta = (Categories = "Stat.PropertyStat"))
	FStatOrValue Min = FStatOrValue(0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attached Property Stats", meta = (Categories = "Stat.PropertyStat"))
	FStatOrValue GainMultiplier = FStatOrValue(1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attached Property Stats", meta = (Categories = "Stat.PropertyStat"))
	FStatOrValue LossMultiplier = FStatOrValue(1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attached Property Stats", meta = (Categories = "Stat.PropertyStat"))
	FStatOrValueWithModificationType FlatBonus = FStatOrValueWithModificationType(0, EModificationType::Addition);
};