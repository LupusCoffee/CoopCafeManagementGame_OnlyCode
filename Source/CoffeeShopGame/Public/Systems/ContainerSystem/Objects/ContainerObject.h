#pragma once

#include "CoreMinimal.h"
#include "Core/Data/DataAssets/ItemData.h"
#include "UObject/Object.h"
#include "ContainerObject.generated.h"


class UResourceConverterSubsystem;
class ULookTraceSubsystem;

UCLASS()
class COFFEESHOPGAME_API UContainerObject : public UObject
{
	GENERATED_BODY()
	

	
public:
	//Methods --> Setup
	UContainerObject();

	UFUNCTION(BlueprintCallable)
	bool SetupData(bool InAcceptsAllResources, TArray<EResourceType> InAcceptedResourceTypes, float InMaxVolume, TArray<FResourceAmount> StarterContents);
	
	
	//Methods --> Adders, Removers, Setters, etc.
	// adders
	UFUNCTION(BlueprintCallable)
	bool TryAddVolume(EResourceType ResourceTypeToAdd, float Value, float& ExcessAmount, bool IgnoreAcceptedResourceTypes);
	UFUNCTION(BlueprintCallable)
	bool AddVolume(EResourceType ResourceTypeToAdd, float PreClampedValueToAdd, float& ExcessAmount);
	
	// removers
	UFUNCTION(BlueprintCallable)
	void RemoveVolumeSequentially(float Value);

	UFUNCTION(BlueprintCallable)
	TArray<FResourceAmount> TakeVolumeSequentially(float Value);
	
	UFUNCTION(BlueprintCallable)
	void RemoveSpecificVolume(EResourceType ResourceTypeToRemove, float Value, float& ExcessAmount);

	UFUNCTION(BlueprintCallable)		
	void RemoveSpecificVolumeFromIndex(int ResourceIndexToRemoveFrom, float Value, float& ExcessAmount);
	
	// setters
	UFUNCTION(BlueprintCallable)
	void SetCurrentVolumeMap(TArray<FResourceAmount> InMap);
	
	UFUNCTION(BlueprintCallable)
	void ZeroVolume();

	UFUNCTION(BlueprintCallable)
	float ZeroSpecificVolume(EResourceType ResourceTypeToZero);

	UFUNCTION(BlueprintCallable)
	float ZeroSpecificVolumeFromIndex(int ResourceIndexToZero);

	
	//Methods --> Getters
	UFUNCTION(BlueprintCallable)
	bool IsFull();

	UFUNCTION(BlueprintCallable)
	float GetMaxVolume() const;

	UFUNCTION(BlueprintCallable)
	float GetCurrentTotalVolume() const;

	UFUNCTION(BlueprintCallable)
	TArray<FResourceAmount> GetCurrentVolumeMap() const;

	UFUNCTION(BlueprintCallable)
	float GetCurrentVolumeOfResource(EResourceType ResourceType);


	
protected:
	//Variables --> Editable
	UPROPERTY(EditAnywhere, BlueprintReadOnly,		Category = "Settings")
	bool bAcceptsAllResources = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,		Category = "Settings")
	TArray<EResourceType> AcceptedResourceTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,		Category = "Settings")
	float MaxVolume = 100.0f;

	
	//Variables --> Visible, Dynamic
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,	Category= "Values")
	float CurrentTotalVolume = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,	Category= "Values")
	TArray<FResourceAmount> CurrentVolumeMap;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,	Category= "Values")
	bool bIsFull = false;



	//Methods --> Setup
	UFUNCTION()
	void SetupResourceMap(TArray<EResourceType> InAcceptedResourceTypes, TArray<FResourceAmount> InStarterContents);

	
	//Methods --> Adders, Removers, Setters, etc.
	// helpers/misc
	UFUNCTION()
	int FindByResourceTypeOrAdd(EResourceType ResourceToFindOrAdd);

	UFUNCTION()
	void UpdateTotalVolume();

	
	//Methods --> Replication
	virtual bool IsSupportedForNetworking() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
