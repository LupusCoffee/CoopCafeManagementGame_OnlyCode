#include "Systems/StatModificationSystem/Components/StatHandlerComponent.h"

#include "Net/UnrealNetwork.h"
#include "Systems/StatModificationSystem/Structs/PropertyStat.h"


//Setup and Such
UStatHandlerComponent::UStatHandlerComponent(): StatsTableData(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UStatHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupStats();
}

bool UStatHandlerComponent::SetupStats()
{
	if (!StatsTableData) return false;
	
	for (auto PropertyStat : StatsTableData->PropertyStats)
		StoredPropertyStats.Add(PropertyStat);

	for (auto MeterStat : StatsTableData->MeterStats)
		StoredMeterStats.Add(MeterStat);
	
	return true;
}


//Adding / Removing
void UStatHandlerComponent::AddStatImpulse(UStatImpulse* StatImpulse)
{
	for (auto StatImpulseEntry : StatImpulse->StatImpulseEntries)
	{
		float CurrentStatValue;
		if (!TryGetModifiedMeterStat(StatImpulseEntry.MeterStat, CurrentStatValue)) CurrentStatValue = 0.0f;
		
		float ValueToApply = Calculate(CurrentStatValue, StatImpulseEntry.ModificationType, StatImpulseEntry.Magnitude) - CurrentStatValue;
		
		ApplyToMeterStatModified(StatImpulseEntry.MeterStat, ValueToApply);
	}
}

void UStatHandlerComponent::AddStatMod(UStatMod* StatMod)
{
	for (auto StatModEntry : StatMod->StatModEntries)
	{
		AddStatEntry(StatModEntry);
		
		OnStatModEntryApplied.Broadcast(StatModEntry.PropertyStat);
	}
	
	StatMods.Add(StatMod);

	OnStatModified.Broadcast();
}

void UStatHandlerComponent::RemoveStatMod(UStatMod* StatMod)
{
	for (auto StatModEntry : StatMod->StatModEntries)
	{
		RemoveStatEntry(StatModEntry);
		
		OnStatModEntryRemoved.Broadcast(StatModEntry.PropertyStat);
	}
	
	StatMods.Remove(StatMod);

	OnStatModified.Broadcast();
}

void UStatHandlerComponent::AddStatEntry(FStatModEntry StatModEntry)
{
	TArray<FStatModEntry>* StatModEntries = FindStatModEntries(StatModEntry.PropertyStat);

	if (StatModEntries)
	{
		StatModEntries->Add(StatModEntry);
	}
	else
	{
		FStatAndItsEntries StatAndItsEntries;
		StatAndItsEntries.Stat = StatModEntry.PropertyStat;
		StatAndItsEntries.StatModEntries.Add(StatModEntry);
		StatsAndTheirEntries.Add(StatAndItsEntries);
	}
}

void UStatHandlerComponent::RemoveStatEntry(FStatModEntry StatModEntry)
{
	TArray<FStatModEntry>* StatModEntries = FindStatModEntries(StatModEntry.PropertyStat);
	
	if (StatModEntries) StatModEntries->Remove(StatModEntry);
}

TArray<FStatModEntry>* UStatHandlerComponent::FindStatModEntries(FGameplayTag Stat)
{
	for (auto& StatAndItsEntries : StatsAndTheirEntries)
	{
		if (!StatAndItsEntries.Stat.MatchesTagExact(Stat)) continue;
		
		return &StatAndItsEntries.StatModEntries;
	}
	
	return nullptr;
}


//Stat Stuff
bool UStatHandlerComponent::TryGetBasePropertyStat(FGameplayTag Stat, float& OutValue)
{
	if (StoredPropertyStats.IsEmpty()) return false;
	
	for (auto BaseStat : StoredPropertyStats)
	{
		if (BaseStat.PropertyStat.MatchesTagExact(Stat))
		{
			OutValue = BaseStat.Value;
			return true;
		}
	}
	return false;
}

bool UStatHandlerComponent::TryGetModifiedPropertyStat(FGameplayTag Stat, float& OutValue)
{
	//todo: figure out a way to store this so we dont have to do the calculation every single time
	
	float BaseStatValue;
	if (!TryGetBasePropertyStat(Stat, BaseStatValue)) return false;

	OutValue = BaseStatValue;

	
	TArray<FStatModEntry>* StatModEntries = FindStatModEntries(Stat);

	if (!StatModEntries) return true;

	
	for (auto StatModEntry : *StatModEntries)
	{
		OutValue = Calculate(OutValue, StatModEntry.ModificationType, StatModEntry.Magnitude);
	}
	
	return true;
}

bool UStatHandlerComponent::TryGetModifiedMeterStat(FGameplayTag Stat, float& OutValue)
{
	if (StoredMeterStats.IsEmpty()) return false;

	for (auto MeterStat : StoredMeterStats)
	{
		if (MeterStat.MeterStat.MatchesTagExact(Stat))
		{
			OutValue = Calculate(MeterStat.Value, MeterStat.FlatBonus.ModificationType,
				MeterStat.FlatBonus.GetValue(this));
			return true;
		}
	}
	
	return false;
}

bool UStatHandlerComponent::ApplyToMeterStatModified(FGameplayTag Stat, float ValueToApply)
{
	if (StoredMeterStats.IsEmpty()) return false;

	for (int i = 0; i < StoredMeterStats.Num(); i++)
	{
		FMeterStat* StoredMeterStat = &StoredMeterStats[i];
			
		if (StoredMeterStat->MeterStat.MatchesTagExact(Stat))
		{
			float Multiplier = 1.0f;
			if (ValueToApply >= 0) Multiplier = StoredMeterStat->GainMultiplier.GetValue(this);
			else Multiplier = StoredMeterStat->LossMultiplier.GetValue(this);
				
			StoredMeterStat->Value += ValueToApply * Multiplier;
			float Max = StoredMeterStat->Max.GetValue(this);
			float Min = StoredMeterStat->Min.GetValue(this);
			StoredMeterStat->Value = FMath::Clamp(StoredMeterStat->Value, Min, Max);
			
			return true;
		}
	}
	
	return false;
}


//Other Utility
float UStatHandlerComponent::Calculate(float ValueA, EModificationType ModType, float ValueB)
{
	switch (ModType)
	{
		case EModificationType::Addition:
			return ValueA + ValueB;

		case EModificationType::Subtraction:
			return ValueA - ValueB;

		case EModificationType::Multiplication:
			return ValueA * ValueB;

		case EModificationType::Division:
			if (ValueB != 0) return ValueA / ValueB;
	}

	return 0;
}

void UStatHandlerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UStatHandlerComponent, StoredMeterStats);
}
