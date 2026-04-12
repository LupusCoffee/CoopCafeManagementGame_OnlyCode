#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Managers/LoopManagementSubsystem.h"
#include "CoffeeShopGame/Managers/TableManagerSubsystem.h"
#include "Controllers/QueueController.h"
#include "Core/Data/Enums/DrinkType.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GOAPSystem/AI/Character/BaseAICharacter.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "Customer.generated.h"

//This should be a container for animations and goap related fields.
//Character should be a data container and generate for other managers and similar to listen to

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInterractionRecievedSignature, ACustomer*, Customer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOrderTaken, EResourceType, DrinkType, AChair*, chair);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrinkOrderGenerated, EResourceType, DrinkOrder);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewTargetAssigned, AActor*, newTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnteredStore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirstInQueue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterQueue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAISatDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAIBumped); // When the AI bumps into someone or another AI



UCLASS()
class COFFEESHOPGAME_API ACustomer : public ABaseAICharacter, public IInteractable
{
public:
	virtual void Tick(float DeltaSeconds) override;

private:
	GENERATED_BODY()

public:
	ACustomer();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	
	virtual void BeginPlay() override;
	
public:
	//===================================================
	//=================== Target code ===================
	//===================================================
	UFUNCTION(BlueprintCallable, Category = "Target")
	void SetTargetActor(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable, Category = "Target")
	AActor* GetTargetActor();
	
	UFUNCTION(BlueprintCallable, Category = "Target")
	void MoveToTargetActor();
	
	UFUNCTION(BlueprintCallable, Category = "Target")
	void MoveToTargetPosition(FVector TargetPosition);
	
	UFUNCTION(BlueprintCallable, Category = "Target")
	bool InteractWithTargetActor(bool bUseGOAPInteract = false);
	
	UFUNCTION(BlueprintCallable, Category = "Target")
	void MoveToTargetActorAndInteract(bool bUseGOAPInteract = false);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	float MovementAcceptanceRadius = 15.0f;
	
	//===================================================
	//================ Table Management =================
	//===================================================
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table")
	int TableId = -1;
	
	UPROPERTY(BlueprintReadOnly, Category = "Customer")
	TWeakObjectPtr<AQueueController> QueueController;

	UFUNCTION()
	void HandleLoopFinished();

	//===================================================
	//================ Customer Actions =================
	//===================================================		
	UFUNCTION(BlueprintCallable, Category="CoffieOrder")
	void GenerateDrinkOrder();
	
	UFUNCTION(BlueprintCallable, Category="CoffieOrder")
	EResourceType GetDrinkOrder() const;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Table")
	bool bIsSitting = false;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Queue")
	bool bIsInQueue = false;
		
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	TObjectPtr<AActor> TargetActor = nullptr;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tips")
	float GenerateTipAmount();
	
	
	
	//===================================================
	//================ Soft Collision =================
	//===================================================	
	
	UPROPERTY(EditAnywhere, Category = "Soft Collision")
	float PushRadius = 80.f;

	UPROPERTY(EditAnywhere, Category = "Soft Collision")
	float PushForce = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soft Collision")
	float BumpCooldown = 2.0f;
	
	//=======================================================
	//==================== Delegates =====================
	//=======================================================
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|CoffieOrder")
	FOnDrinkOrderGenerated OnDrinkGenerated;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|CoffieOrder")
	FOnOrderTaken OnOrderTaken;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Target")
	FOnNewTargetAssigned OnNewTargetAssigned;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Coffee Loop")
	FOnInterractionRecievedSignature OnInteractionReceived;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Coffee Loop")
	FOnEnteredStore OnEnteredStore;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Actions")
	FOnMoveStarted OnMoveStarted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Queue")
	FOnFirstInQueue OnFirstInQueue;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Queue")
	FOnEnterQueue OnEnterQueue;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Actions")
	FOnAISatDown OnAISatDown;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AI|Soft Collision")
	FOnAIBumped OnAIBumped;

private:	
	UFUNCTION(Server, Reliable)
	void Server_ProcessInteraction(bool GOAPInteract);

	UFUNCTION(Server, Reliable)
	void Server_SetTargetActor(AActor* InTargetActor);

	UFUNCTION(Server, Reliable)
	void Server_MoveToTargetActor();
	
	UFUNCTION(Server, Reliable)
	void Server_MoveToTargetPosition(FVector targetPosition);

	UFUNCTION(Server, Reliable)
	void Server_MoveToTargetActorAndInteract(bool GOAPInteract);

	void ProcessInteractionOnServer(bool GOAPInteract);

	UFUNCTION()
	void PrivateInteractWIthTargetActor(ACustomer* FinishedCustomer);
	
	UFUNCTION()
	void GetFreeChair();
	
	UPROPERTY()
	bool bIsMovingToTargetActor = false;
	
	UPROPERTY()
	ULoopManagementSubsystem* LoopManagementSubsystem;
	UPROPERTY()
	UTableManagerSubsystem* TableManagerSubsystem;
	
	UPROPERTY(Replicated)
	EResourceType DesiredDrink = EResourceType::BasicCoffee;
	
	UPROPERTY()
	bool bUseGOAPInteractForPendingMove = false;

    float LastBumpTime = -FLT_MAX;
};
