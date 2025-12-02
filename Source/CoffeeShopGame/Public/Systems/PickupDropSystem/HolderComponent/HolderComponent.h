#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/PickupDropSystem/Structs/HeldItem.h"
#include "HolderComponent.generated.h"
class ULookTraceSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, UHeldItem*, HeldItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThrowEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnThrowItemTick, float, CurrentThrowForce, float, MinThrowForce, float, MaxThrowForce);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UHolderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHolderComponent();

	//Methods --> Pick Up Item, Replicated
	UFUNCTION(BlueprintCallable, Category="Holder") //perhaps this should be removed?
	void PickUpItem(UHeldItem* ItemToPickUp);

	
	//Methods --> Drop Item, Replicated
	UFUNCTION(BlueprintCallable, Category="Holder")	//perhaps this should be removed?
	void DropItem(FVector InLookedAtLocation);


	//Methods --> Attach Item, Replicated
	UFUNCTION(BlueprintCallable, Category="Holder")
	void AttachItemToSocket(UStaticMeshComponent* MeshWithSocket, FName SocketName);

	UFUNCTION(BlueprintCallable, Category="Holder")
	void DetachItemStillAsHeld(bool bResetPhysicsToInitialState);

	UFUNCTION(BlueprintCallable, Category="Holder")
	void ReattachItemStillAsHeld();
	

	//Methods --> Throw Item, Replicated --> todo: refactor this a lil bit (_controller and GetThrowDir in ServerThrowItem smh)
	UFUNCTION(BlueprintCallable, Category="Thrower")
	void ThrowItem(AController* _Controller, float ThrowForce);

	UFUNCTION()
	FVector GetPreThrowLocation(AActor* HeldItemActor, AController* _Controller);
	
	UFUNCTION()
	FVector GetThrowDir(AController* _Controller);

	UFUNCTION(BLueprintCallable, Category="Thrower")
	void ServerSide_ThrowItemStart();

	UFUNCTION(Client, Reliable)
	void Client_ThrowItemStart();

	UFUNCTION(BLueprintCallable, Category="Thrower")
	void ServerSide_ThrowItemCompleted(AController* InController);

	UFUNCTION(Client, Reliable)
	void Client_ThrowItemCompleted();
	

	//Methods --> Getters
	UFUNCTION(BlueprintCallable, Category="Holder")
	UHeldItem* GetHeldItem();

	UFUNCTION(BlueprintCallable, Category="Holder")
	bool IsHolding();

	UFUNCTION()
	FVector GetLookedAtSurfaceLocation();

	
protected:
	//Overrides
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Variables --> Static, SetValues
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Thrower")
	float ThrowForceMultiplier = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Thrower")
	float MaxThrowForce = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Thrower")
	float MinThrowForce = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Thrower")
	float ThrowAngle = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Thrower")
	float ForwardStartPos = 10.0f;

	
	//Variables --> Dyanamic, Tick
	UPROPERTY(BlueprintReadOnly, Category="Thrower")
	float ServerSideStartThrowTimeStamp = 0;

	UPROPERTY(BlueprintReadOnly, Category="Thrower")
	float ClientSideCurrentThrowForce = 0;

	UPROPERTY(BlueprintReadOnly, Category="Thrower")
	bool bClientSideWantsToTickThrow = false;
	
	
	//Variables --> Static, General
	UPROPERTY()
	APawn* Pawn = nullptr;
	
	UPROPERTY()
	AController* Controller = nullptr;
	
	UPROPERTY()
	ULookTraceSubsystem* LookTraceSubsystem = nullptr;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HeldItemUpdate)
	TObjectPtr<UHeldItem> HeldItem;

	UPROPERTY(Replicated)
	bool bIsHolding = false;

	UPROPERTY(Replicated)
	bool bIsAttaching = false;

	
	//Variables --> Static, Socket Attachment
	UPROPERTY()
	USkeletalMeshComponent* OwnerMesh = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Holder")
	FName HeldItemSocketName;


	//Methods
	UFUNCTION()
	bool TryInitTraceSubsystem();

	//Methods --> Updates
	UFUNCTION(BLueprintCallable, Category="Thrower")
	void ClientSide_ThrowItemTick(float DeltaTime);

	//Methods --> Replication, Multicast
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdatePrimitiveComponentCollision(UPrimitiveComponent* PrimitiveComponent, ECollisionEnabled::Type Col);

	//Methods --> Replication, OnRep
	UFUNCTION()	//explanation: create replicated stuff on Server
	void OnRep_HeldItemUpdate(UHeldItem* LastHeldItem);

	//Methods --> Replication, Setup
	//explanation: where you record properties of your actor class that you want to replicate
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()	//explanation: create replicated stuff on Server
	void BeginPlaySetupForReplicatedObjects();


	//Delegates --> Variables, Attach/Detach Item
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnAttachItem;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnDetachItem;

	
	//Delegates --> Variables, Throw
	UPROPERTY(BlueprintAssignable)
	FOnThrowEvent OwnerClient_OnThrowStart;

	UPROPERTY(BlueprintAssignable)
	FOnThrowEvent OwnerClient_OnThrowEnd;

	UPROPERTY(BlueprintAssignable)
	FOnThrowItemTick OwnerClient_OnThrowItemTick;
	

	//Delegates --> Methods
	UFUNCTION()
	void OnOwnerPawnControllerChanged(APawn* InPawn, AController* OldController, AController* NewController);
};
