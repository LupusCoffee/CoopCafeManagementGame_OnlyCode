// Fill out your copyright notice in the Description page of Project Settings.


#include "Queue.h"

#include "CoffeeShopGame/Customer/Controllers/QueueController.h"
#include "CoffeeShopGame/Managers/LoopManagementSubsystem.h"
#include "CoffeeShopGame/Customer/Customer.h"
#include "Components/SplineComponent.h"
#include "GOAPSystem/AI/Controller/BaseAiController.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AQueue::AQueue()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	ChairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairMesh"));
	SetRootComponent(ChairMesh);

	QueueSpline = CreateDefaultSubobject<USplineComponent>(TEXT("QueueSpline"));
	QueueSpline->SetupAttachment(RootComponent);
}

void AQueue::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQueue, NumQueuePositions);
	DOREPLIFETIME(AQueue, QueuePositions);
	DOREPLIFETIME(AQueue, CustomerList);
}

// Called when the game starts or when spawned
void AQueue::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		GenerateQueuePositions();
	}
	
	if (auto* LoopManager = GetWorld()->GetSubsystem<ULoopManagementSubsystem>())
		LoopManager->SetQueueReference(this);
	
}

// Called every frame
void AQueue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AQueue::AddCustomer(ACustomer* customer)
{
	if (!HasAuthority())
	{
		if (IsValid(customer))
		{
			Server_AddCustomer(customer);
		}
		return false;
	}

	return AddCustomerInternal(customer);
}

bool AQueue::AddCustomerInternal(ACustomer* customer)
{
	if (!IsValid(customer))
	{
		return false;
	}

	if (CustomerList.Contains(customer) || !CanAddCustomer())
	{
		return false;
	}

	if (AQueueController* AIController = Cast<AQueueController>(customer->GetController()))
	{
		CustomerList.Add(customer);
		AIController->MoveToPosition(QueuePositions[CustomerList.IndexOfByKey(customer)], 5.0f, EMovePriority::LowPriority);
		customer->bIsInQueue = true;
		customer->OnEnterQueue.Broadcast();
		
		if (GetPositionInQueue(customer) == 0)
		{
			customer->OnFirstInQueue.Broadcast();
		}
		
		return true;
	}

	return false;
}

void AQueue::Server_AddCustomer_Implementation(ACustomer* Customer)
{
	AddCustomerInternal(Customer);
}

void AQueue::RelocateAllCustomers()
{
	for (auto customer : CustomerList)
	{
		if (!IsValid(customer)) continue;
		
		if (ABaseAiController* AIController = Cast<ABaseAiController>(customer->GetController()))
		{
			AIController->MoveToPosition(QueuePositions[CustomerList.IndexOfByKey(customer)], 5.0f, EMovePriority::LowPriority);
		}
		
		if (GetPositionInQueue(customer) == 0)
		{
			customer->OnFirstInQueue.Broadcast();
		}
	}
}

bool AQueue::CanAddCustomer()
{
	return CustomerList.Num() < QueuePositions.Num();
}

void AQueue::GenerateQueuePositions()
{
	if (!HasAuthority())
	{
		return;
	}

	QueuePositions.Reset();

	if (!IsValid(QueueSpline))
	{
		NumQueuePositions = 0;
		return;
	}

	const float QueueSpacing = FMath::Max(1.0f, DistanceBetweenQueuePositions);
	const float SplineLength = QueueSpline->GetSplineLength();

	NumQueuePositions = FMath::Max(FMath::FloorToInt(SplineLength / QueueSpacing) + 1, 1);
	QueuePositions.Reserve(NumQueuePositions);

	for (int i = 0; i < NumQueuePositions; i++)
	{
		const float DistanceOnSpline = FMath::Min(i * QueueSpacing, SplineLength);
		QueuePositions.Add(QueueSpline->GetLocationAtDistanceAlongSpline(DistanceOnSpline, ESplineCoordinateSpace::World));
	}

	for (auto position : QueuePositions)
	{
		DrawDebugCircle(
		   GetWorld(),
		   position,
		   10,
		   32,
		   FColor::Green,
		   true,
		   2,
		   0,
		   10.0f,     // Thickness
		   FVector(1, 0, 0),  // Axis X
		   FVector(0, 1, 0),  // Axis Y
		   true               // Draw full circle (not arc)
	   );
	}
	
}

int AQueue::GetPositionInQueue(ACustomer* customer) const
{
	if (customer == nullptr)
		return INDEX_NONE;
		
	return CustomerList.IndexOfByKey(customer);
}

void AQueue::CustomerLeavesQueue(ACustomer* customer)
{	
	if (!HasAuthority())
	{
		if (IsValid(customer))
		{
			Server_CustomerLeavesQueue(customer);
		}
		return;
	}

	CustomerLeavesQueueInternal(customer);
}

void AQueue::CustomerLeavesQueueInternal(ACustomer* customer)
{
	if (!IsValid(customer))
	{
		return;
	}

	const int CustomerIndex = CustomerList.IndexOfByKey(customer);
	if (CustomerIndex == INDEX_NONE)
	{
		return;
	}

	customer->bIsInQueue = false;
	CustomerList.RemoveAt(CustomerIndex);
	RelocateAllCustomers();
}

void AQueue::Server_CustomerLeavesQueue_Implementation(ACustomer* Customer)
{
	CustomerLeavesQueueInternal(Customer);
}

