#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PropertyStat.generated.h"

USTRUCT(BlueprintType)
struct FPropertyStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stat.PropertyStat"))
	FGameplayTag PropertyStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;
};
