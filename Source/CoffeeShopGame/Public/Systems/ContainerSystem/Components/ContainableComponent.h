#pragma once


//includes + delegates
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Data/DataAssets/ItemData.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "Engine/EngineTypes.h"
#include "ContainableComponent.generated.h"
class UNiagaraSystem;
class UContainerObject;
class UResourceConverterSubsystem;
class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPourStart, AActor*, PourInteractor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPourEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAddVolume, bool, bSuccess, float, PouredVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainerEmpty);


//main
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UContainableComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

	
public:
	//Basic Setup + Tick
	UContainableComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	//Methods, Interaction
	UFUNCTION()
	bool StartHeldPourInteraction(AActor* Interactor, UHolderComponent* HolderComp, UStatHandlerCompatibleCharacterMovementComponent* MovementComponent, AActor* PourTarget);

	UFUNCTION()
	bool TickHeldPourInteraction(UHolderComponent* HolderComp, UStatHandlerComponent* StatHandler, float DeltaTime);

	UFUNCTION()
	bool EndHeldPourInteraction(UHolderComponent* HolderComp, UStatHandlerCompatibleCharacterMovementComponent* MovementComponent);


	//Methods, Utilities (Pouring)
	UFUNCTION(BlueprintCallable)
	void TryMovePourerToTargetOrAnchor(UHolderComponent* HolderComp, AActor* Pourer, AActor* PourTarget);

	UFUNCTION(BlueprintCallable)
	void ReturnPourerToHand(UHolderComponent* HolderComp);

	UFUNCTION(BlueprintCallable)
	void PourDownFromActor(AActor* InActor, float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void PourIntoTarget(AActor* PourTarget, float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EResourceType GetPouringResourceType();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	USceneComponent* GetPourAnchor() const;

	
	//Methods, Utilities (Conversion)
	UFUNCTION(BlueprintCallable)
	bool TryMixContents();

	UFUNCTION(BlueprintCallable)
	void TryGrindContents();

	
	//Methods, Utilities (Volume)
	// add / remove
	UFUNCTION(BlueprintCallable)
	bool TryAddVolume(EResourceType ResourceTypeToAdd, float Value, float& ExcessAmount, bool IgnoreAcceptedResourceTypes);
	
	UFUNCTION(BlueprintCallable)
	void RemoveVolumeSequentially(float Value);

	// set / empty
	UFUNCTION(BlueprintCallable)
	void SetCurrentVolumeMap(TArray<FResourceAmount> InMap);
	
	UFUNCTION(BlueprintCallable)
	void EmptyVolume();

	UFUNCTION(BlueprintCallable)
	float EmptySpecificVolume(EResourceType ResourceTypeToRemove);

	UFUNCTION(BlueprintCallable)
	float EmptySpecificVolumeFromIndex(int ResourceIndexToRemoveFrom);

	// is / getters
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UContainerObject* GetContainerObject();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsFull();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentTotalVolume();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FResourceAmount> GetCurrentVolumeMap();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentVolumeOfResource(EResourceType ResourceType);
	

	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnPourStart OnStartPour;

	UPROPERTY(BlueprintAssignable)
	FOnPourEnd OnEndPour;

	UPROPERTY(BlueprintAssignable)
	FOnAddVolume OnAddVolume;

	UPROPERTY(BlueprintAssignable)
	FOnContainerEmpty OnContainerEmpty;

	UPROPERTY(BlueprintAssignable)
	FOnContainerEmpty OnContainerNotEmpty;

	
protected:
	//Variables, Actors (Hidden)
	UPROPERTY()
	AActor* Owner = nullptr;
	
	//Variables, Components (Hidden)
	UPROPERTY()
	UStaticMeshComponent* MeshComp = nullptr;

	//Variables, Subsystems (Hidden)
	UPROPERTY()
	ULookTraceSubsystem* LookTraceSubsystem = nullptr;

	UPROPERTY()
	UResourceConverterSubsystem* ResourceConverterSubsystem = nullptr;


	//Variables, Data (Editable)
	UPROPERTY(EditDefaultsOnly, Category="Containable | Data")
	UContainerData* ContainerData = nullptr;


	//Variables, Container (Editable)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Container")
	bool bAcceptsAllResources = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Container")
	TArray<EResourceType> AcceptedResourceTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Container")
	TArray<FResourceAmount> StarterContents;


	//Variables, Volume (Editable)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Volume")
	float MaxVolume = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Volume")
	float DecayVolumeRate = 0;
	
	//Variables, Volume (Dynamic)
	UPROPERTY(Replicated, VisibleInstanceOnly, Instanced, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	TObjectPtr<UContainerObject> ContainerObject = nullptr;

	UPROPERTY()
	bool bEmptyContainerBroadcasted = false;


	//Variables, Conversion (Editable)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Conversion")
	bool bConvertsContentsToRecipe = true;
	
	
	//Variables, Pouring (Editable)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring")
	bool bCanPour = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring")
	bool bCanBePouredInto = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring")
	float BasePourRate = 90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring")
	EResourceType BasePouringResource = EResourceType::BasicCoffee;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring")
	bool bUseContentsAsPouringResource = true;

	UPROPERTY(Transient)
	USceneComponent* PourAnchor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Containable")
	TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring | tracing stuff")
	float HeldPouringHeight = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring | tracing stuff")
	float PourDownTraceLength = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring | tracing stuff")
	float PourDownTraceRadius = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Containable | Pouring | tracing stuff")
	TEnumAsByte<ECollisionChannel> PourDownTraceChannel = ECollisionChannel::ECC_Visibility;

	//Variables, Pouring (Dynamic)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentPourRate = 90;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* PourTargetAsMovedOverObjectCached = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* PourTargetCached = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UContainableComponent* PourTargetContainerCached = nullptr;

	UPROPERTY()
	FName PourAnchorName;

	UPROPERTY()
	FName PourNiagaraName;


	//Variables, Visuals (Dynamic)
	UPROPERTY(VisibleAnywhere)
	FRotator InitialRotation = FRotator::ZeroRotator;


	//Methods/Variables, Setup (Custom Editor)
#if WITH_EDITOR
	virtual void OnComponentCreated() override;
	virtual void PostEditUndo() override;
	
	void EnsurePourAnchorExists();
	bool bIsEnsuringPourAnchor = false;
	void EnsurePourNiagaraExists();
	bool bIsEnsuringPourNiagara = false;
#endif

	
	//Methods, Setup (BeginPlay)
	UFUNCTION()
	void SetupVariables();

	UFUNCTION()
	bool TrySetupVariablesWithContainerData();

	UFUNCTION()
	void SetupContainerObject();
	
	
	//Methods, Interactions (Pouring)
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext) override;
	virtual bool Server_TickInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext, float DeltaTime) override;
	virtual bool Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext) override;


	//Methods, Utilities (Helper)
	UFUNCTION(BlueprintCallable)
	void MovePourerToTarget(AActor* Pourer, AActor* PourTarget);


	//Methods, Replication
	UFUNCTION()	//explanation: create replicated stuff on Server
	void SetupForReplicatedObjects();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
	//Methods, Visuals
	UFUNCTION(NetMulticast, Reliable)
	void StartPourVisuals(AActor* PourInteractor);
	
	UFUNCTION(NetMulticast, Reliable)
	void EndPourVisuals();

	UFUNCTION(NetMulticast, Reliable)
	void EmptiedDuringPourVisuals();
};
