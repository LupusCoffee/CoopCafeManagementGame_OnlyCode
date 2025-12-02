#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Systems/StatModificationSystem/Enums/ModificationType.h"
#include "StatOrValueWithModificationType.generated.h"
class UStatHandlerComponent;

USTRUCT(BlueprintType)
struct FStatOrValueWithModificationType
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	bool bUseLiteralValue = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!bUseLiteralValue", Categories="Stat.PropertyStat"))
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseLiteralValue"))
	float LiteralValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EModificationType ModificationType = EModificationType::Addition;
	
	bool UsesStat() const { return !bUseLiteralValue && StatTag.IsValid(); }

	float GetValue(UStatHandlerComponent* StatSystem) const;

	FStatOrValueWithModificationType() = default;
	FStatOrValueWithModificationType(float DefaultLiteralValue, EModificationType DefaultModificationType)
	{
		LiteralValue = DefaultLiteralValue;
		ModificationType = DefaultModificationType;
	};
};