// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Customer/Queue/Queue.h"
#include "GOAPSystem/AI/Controller/BaseAiController.h"
#include "QueueController.generated.h"

class ACustomer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveCompleted, ACustomer*, Customer);

UCLASS()
class COFFEESHOPGAME_API AQueueController : public ABaseAiController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQueueController();
	
	UFUNCTION(BlueprintCallable)
	ACustomer* GetCustomer() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Movement")
	FOnMoveCompleted MoveCompleted;
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(BlueprintCallable, Category="Queue")
	AQueue* GetQueue();
	
	UFUNCTION(BlueprintCallable, Category="Queue")
	void SetQueue(AQueue* queue);
	
	UFUNCTION(BlueprintCallable, Category="Queue")
	bool IsInQueue();
private:
	UPROPERTY()
	ACustomer* Customer;
	
	UPROPERTY()
	TWeakObjectPtr<AQueue> CurrentQueue;
};
