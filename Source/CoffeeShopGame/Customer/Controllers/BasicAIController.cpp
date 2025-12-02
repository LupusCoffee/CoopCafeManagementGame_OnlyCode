// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "CoffeeShopGame/Customer/Customer.h"
#include "Core/HelperCode.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Systems/StatModificationSystem/GameTags.h"

FOrderData::FOrderData()
{
	GenerateOrder();
}

void FOrderData::GenerateOrder()
{
	DrinkType = GetRandomEnumValue<EDrinkType>();
}

// Sets default values
ABasicAIController::ABasicAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GOAP = CreateDefaultSubobject<UGOAP_Component>(TEXT("GOAP_Component"));
	TimerComponent = CreateDefaultSubobject<UTimerComponent>(TEXT("Timer Component"));
}

// Called when the game starts or when spawned
void ABasicAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABasicAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (IsValid(InPawn))
	{
		auto cust = Cast<ACustomer>(InPawn);
		cust->SetBasicAIController(this);
		StatHandler = cust->GetComponentByClass<UStatHandlerComponent>();
	}
}

// Called every frame
void ABasicAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TimerStarted)
	{
		float PatienceTime = 0;
		StatHandler->TryGetModifiedMeterStat(TAG_MeterStat_Customer_PatienceTime,PatienceTime);		
	}
}

bool ABasicAIController::TakeOder()
{
	if (!GetBlackboardComponent()->GetValueAsBool(FName("HasOrdered")))
	{
		GetBlackboardComponent()->SetValueAsBool(FName("HasOrdered"), true);
		GOAP->CurrentWorldState.HasOrdered = true;

		if (IsValid(CurrentQueue))
		{
			CurrentQueue->CustomerLeavesQueue(GetPawn());
			RemoveQueue();
		}

		return true;
	}
	return false;
}

bool ABasicAIController::AddQueue(AQueue* queue)
{
	if (CurrentQueue == nullptr)
	{
		CurrentQueue = queue;
		return true;
	}
	return false;
}

bool ABasicAIController::RemoveQueue()
{
	if (CurrentQueue != nullptr)
	{
		CurrentQueue = nullptr;
		return true;
	}
	return false;
}

void ABasicAIController::GenerateOrder()
{
	DrinkOrder.GenerateOrder();
}

void ABasicAIController::CustomersLostPatience()
{
	
	GOAP->CurrentWorldState.Thirstiness = 0.0f;
	GOAP->CurrentWorldState.HasOrdered = false;
	GOAP->CurrentWorldState.HasCoffee = false;

	if (IsValid(CurrentQueue))
	{
		CurrentQueue->CustomerLeavesQueue(GetPawn());
		RemoveQueue();
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "arrrrgh, you have angered me!!!! im gona leave!!!");
	}
	
	GOAP->EvaluateGoals();
}

void ABasicAIController::TickThirsty()
{
	GOAP->CurrentWorldState.Thirstiness += FMath::RandRange(minThirstyTick, maxThirstyTick);
	if (GOAP->CurrentWorldState.Thirstiness >= 1.0f)
	{
		GOAP->CurrentWorldState.Thirstiness = 1.0f;
		GOAP->EvaluateGoals();
	}
}

