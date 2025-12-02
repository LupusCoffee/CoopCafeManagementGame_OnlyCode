// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CoffeeShopGame/Customer/GOAP/GOAP_Component.h"
#include "CoffeeShopGame/Customer/Misq/TimerComponent.h"
#include "CoffeeShopGame/Customer/Queue/Queue.h"
#include "Core/Data/Enums/DrinkType.h"
#include "Systems/StatModificationSystem/Components/StatHandlerComponent.h"
#include "BasicAIController.generated.h"

USTRUCT(BlueprintType)
struct FOrderData
{
	GENERATED_BODY()

	FOrderData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
	EDrinkType DrinkType;
	
	void GenerateOrder();
};

USTRUCT(BlueprintType)
struct FTimer
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float MaxDuration = 0.0f;
};

UCLASS()
class COFFEESHOPGAME_API ABasicAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasicAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Ai")
	bool TakeOder();

	UFUNCTION(BlueprintCallable, Category = "Queue")
	bool AddQueue(AQueue* queue);

	UFUNCTION(BlueprintCallable, Category = "Queue")
	bool RemoveQueue();

	UFUNCTION(BlueprintCallable, BlueprintCallable)
	void GenerateOrder();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGOAP_Component* GOAP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTimerComponent* TimerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AQueue* CurrentQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOrderData DrinkOrder;

	UFUNCTION(BlueprintCallable, Category = "Timer|Patience")
	void CustomersLostPatience();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Patience")
	FTimer PatienceTimerStats;

public:
	UFUNCTION(BlueprintCallable, Category = "Behaviour")
	void TickThirsty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thirsty tick")
	float minThirstyTick = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thirsty tick")
	float maxThirstyTick = 0.2f;

private:

	UPROPERTY()
	bool TimerStarted = false;
	
	UPROPERTY()
	UStatHandlerComponent* StatHandler;
};
