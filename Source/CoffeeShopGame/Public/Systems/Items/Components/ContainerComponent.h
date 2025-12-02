#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Data/DataAssets/ItemData.h"
#include "Systems/Items/Enums/ResourceType.h"
#include "Systems/Items/Structs/ResourceStack.h"
#include "Systems/Items/Subsystems/RecipeSubsystem.h"
#include "Systems/PickupDropSystem/HolderComponent/HolderComponent.h"
#include "ContainerComponent.generated.h"
struct FResourcePercentage;
class ULookTraceSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAddVolume, bool, bSuccess, float, PouredVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPourIntoEmptySpot, float, PouredVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainerEmpty);

USTRUCT(BlueprintType)
struct FCurrentResourcePercentage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType ResourceType = EResourceType::CoffeeBeans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentPercentage = 0.0f;

	FCurrentResourcePercentage() = default;
	FCurrentResourcePercentage(EResourceType InResourceType, float CurrentVolume, float MaxVolume)
	{
		ResourceType = InResourceType;
		CurrentPercentage = CurrentVolume/MaxVolume;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//Constructor, Setup, and Overrides
	UContainerComponent();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool SetupContainerData(UContainerData* ContainerData);


	//Tick
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	//Methods --> volume: setters, adders, removers, etc.
	UFUNCTION(BlueprintCallable)
	bool TryAddVolume(EResourceType ResourceTypeToAdd, float Value);

	UFUNCTION(BlueprintCallable)
	void RemoveVolume(float Value);

	UFUNCTION(BlueprintCallable)
	void ZeroVolume();

	UFUNCTION(BlueprintCallable)
	bool IsFull();

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool IsAvailableForAI();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void AIPickUp(bool pickedUp);
	
	//Methods --> pouring
	UFUNCTION()
	void PourIntoMovedOverTargetStart(UHolderComponent* HolderCompForPourer, AActor* PourTargetActor, float InPourRate);

	UFUNCTION()
	void PourIntoMovedOverTargetEnd(UHolderComponent* HolderCompForPourer);

	UFUNCTION(BlueprintCallable)
	void SetPourRate(float InPourRate);
	
	UFUNCTION(BlueprintCallable)
	void PourDownTick(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void PourIntoTargetTick(float DeltaTime, AActor* PourTarget);
	
	//Methods --> getters
	UFUNCTION()
	float GetCurrentTotalVolume();

	UFUNCTION()
	float GetCurrentVolume(EResourceType ResourceType);

	UFUNCTION()
	EResourceType GetPouringResourceType();

	
	//Delegates --> Variables, Attach/Detach Item
	UPROPERTY(BlueprintAssignable)
	FOnAddVolume OnAddVolume;

	UPROPERTY(BlueprintAssignable)
	FOnPourIntoEmptySpot OnPourIntoEmptySpot;

	UPROPERTY(BlueprintAssignable)
	FOnContainerEmpty OnContainerEmpty;

	UPROPERTY(BlueprintAssignable)
	FOnContainerEmpty OnContainerNonEmpty;

protected:
	//Variables --> Editable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,	Category = "Settings | Not Data Settable | Container")
	TArray<FResourceStack> StarterContents;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,		Category = "Settings | Data Settable | Container")
	TArray<EResourceType> AcceptedResourceTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,		Category = "Settings | Data Settable | Container")
	float MaxVolume = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,		Category = "Settings | Data Settable | Container")
	float DecayVolumeOverTimeRate = 0.0f;

	UPROPERTY(EditAnywhere,							Category = "Settings | Data Settable | Pouring")
	float HeightOverTargetUponPouringInto = 30.0f;
	
	UPROPERTY(EditAnywhere,							Category = "Settings | Data Settable | Pouring")
	float PourDownTraceLength = 200.0f;

	UPROPERTY(EditAnywhere,							Category = "Settings | Data Settable | Pouring")
	float PourDownTraceRadius = 1.0f;

	UPROPERTY(EditAnywhere,							Category = "Settings | Data Settable | Pouring")
	TEnumAsByte<ECollisionChannel> PourDownTraceChannel = ECollisionChannel::ECC_Visibility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,	Category = "Settings | Data Settable | Pouring")
	EResourceType PouringResourceType = EResourceType::CoffeePowder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,	Category = "Settings | Data Settable | Recipe")
	bool bConvertsResourcesToRecipe = true;

	
	//Variables --> Visible, Dynamic
	// setup
	UPROPERTY()
	bool bHasInitializedData = false;

	// continuous
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,	Category= "Values | Volume")
	float CurrentTotalVolume = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,	Category= "Values | Volume") //fix it so it changes on rep
	TArray<FResourceStack> CurrentVolumeMap;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,	Category= "Values | Volume")
	bool bIsFull = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,				Category= "Values | Recipe")
	TArray<FCurrentResourcePercentage> CurrentRecipe;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,				Category= "Values | Pouring")
	float PourRate = 1.0f;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,	Category = "Values | AI")
	bool AIPickedUp = false;

	UPROPERTY()
	bool bBroadCastEmptyContainer = false;


	//Variables --> Hidden, Components and Subsystems
	UPROPERTY()
	ULookTraceSubsystem* LookTraceSubsystem = nullptr;

	UPROPERTY()
	URecipeSubsystem* RecipeSubsystem = nullptr;


	//Methods --> Logic
	UFUNCTION(BlueprintCallable)
	bool AddVolume(EResourceType ResourceTypeToAdd, float Value);

	//Methods --> Helpers
	UFUNCTION()
	bool TryInitializeData();
	
	UFUNCTION()
	void SetupResourceMap();

	UFUNCTION()
	void UpdateTotalVolume();

	UFUNCTION()
	void UpdateCurrentRecipe();

	UFUNCTION()
	void UpdateContent(EResourceType NewResourceType);


	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
