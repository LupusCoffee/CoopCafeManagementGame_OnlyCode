#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Systems/StatModificationSystem/Structs/MeterStat.h"
#include "Systems/StatModificationSystem/Structs/StatOrValue.h"
#include "StatsTable.generated.h"
struct FPropertyStat;


UCLASS()
class COFFEESHOPGAME_API UStatsTable : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ToolTip = "Stats that can only be gotten, never added or removed from. This is what we apply modifiers to."))
	TArray<FPropertyStat> PropertyStats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ToolTip = "Stats that can be added and removed from. The way they change are affected by thier property stats."))
	TArray<FMeterStat> MeterStats;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};


inline void UStatsTable::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

#if WITH_EDITOR
	FProperty* MemberProperty = PropertyChangedEvent.MemberProperty;
	if (!MemberProperty) return;
	
	if (MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FStatOrValue, StatTag))
	{
		auto FixupNoModType = [](FStatOrValue& Value)
		{
			Value.bUseLiteralValue = !Value.StatTag.IsValid();
		};

		auto FixupWithModType = [](FStatOrValueWithModificationType& Value)
		{
			Value.bUseLiteralValue = !Value.StatTag.IsValid();
		};
		
		for (FMeterStat& Meter : MeterStats)
		{
			FixupNoModType(Meter.Max);
			FixupNoModType(Meter.Min);
			FixupNoModType(Meter.GainMultiplier);
			FixupNoModType(Meter.LossMultiplier);
			FixupWithModType(Meter.FlatBonus);
		}
	}
#endif
}
