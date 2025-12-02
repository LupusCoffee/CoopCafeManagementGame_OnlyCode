#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "StatOrValue.generated.h"
class UStatHandlerComponent;

USTRUCT(BlueprintType)
struct FStatOrValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	bool bUseLiteralValue = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!bUseLiteralValue", Categories="Stat.PropertyStat"))
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseLiteralValue"))
	float LiteralValue = 0.0f;
	
	bool UsesStat() const { return !bUseLiteralValue && StatTag.IsValid(); }

	float GetValue(UStatHandlerComponent* StatSystem) const;

	FStatOrValue() = default;
	FStatOrValue(float DefaultLiteralValue)
	{
		LiteralValue = DefaultLiteralValue;
	};
};
