#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/StatModificationSystem/StatImpulse.h"
#include "Systems/StatModificationSystem/Enums/ModificationType.h"
#include "Systems/StatModificationSystem/StatMod.h"
#include "Systems/StatModificationSystem/Data/StatsTable.h"
#include "StatHandlerComponent.generated.h"
struct FStatEntryValues;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatModified);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatModChanged, FGameplayTag, Stat);

USTRUCT(BlueprintType)
struct FStatAndItsEntries
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStatModEntry> StatModEntries;

	FStatAndItsEntries() = default;
	FStatAndItsEntries(FGameplayTag InStat, TArray<FStatModEntry> InStatModEntries)
	{
		Stat = InStat;
		StatModEntries = InStatModEntries;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UStatHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatHandlerComponent();
	virtual void BeginPlay() override;

	//Methods
	UFUNCTION()
	void AddStatImpulse(UStatImpulse* StatImpulse);
	
	UFUNCTION()
	void AddStatMod(UStatMod* StatMod);

	UFUNCTION()
	void RemoveStatMod(UStatMod* StatMod);

	UFUNCTION(BlueprintCallable)
	bool TryGetBasePropertyStat(FGameplayTag Stat, float& OutValue);

	UFUNCTION(BlueprintCallable)
	bool TryGetModifiedPropertyStat(FGameplayTag Stat, float& OutValue);

	UFUNCTION(BlueprintCallable)
	bool TryGetModifiedMeterStat(FGameplayTag Stat, float& OutValue);

	UFUNCTION(BlueprintCallable)
	bool ApplyToMeterStatModified(FGameplayTag Stat, float ValueToApply);

	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnStatModified OnStatModified;
	
	UPROPERTY(BlueprintAssignable)
	FOnStatModChanged OnStatModEntryApplied;

	UPROPERTY(BlueprintAssignable)
	FOnStatModChanged OnStatModEntryRemoved;

protected:
	//Variables, Editable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Stat Mods that have modified this Stat Handler's owner"))
	UStatsTable* StatsTableData;
	
	//Variables, Visible
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FPropertyStat> StoredPropertyStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<FMeterStat> StoredMeterStats;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ToolTip = "Stat Mods that have modified this Stat Handler's owner"))
	TArray<UStatMod*> StatMods;

	UPROPERTY(VisibleAnywhere)
	TArray<FStatAndItsEntries> StatsAndTheirEntries;


	//Methods, Setup
	UFUNCTION()
	bool SetupStats();
	

	//Methods, Utility
	UFUNCTION()
	void AddStatEntry(FStatModEntry StatModEntry);

	UFUNCTION()
	void RemoveStatEntry(FStatModEntry StatModEntry);
	
	TArray<FStatModEntry>* FindStatModEntries(FGameplayTag Stat);

	UFUNCTION()
	float Calculate(float ValueA, EModificationType ModType, float ValueB);


	//Methods, Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
