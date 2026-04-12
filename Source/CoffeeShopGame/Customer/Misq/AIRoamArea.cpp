// Fill out your copyright notice in the Description page of Project Settings.


#include "AIRoamArea.h"
#include "../Customer/Controllers/QueueController.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "TimerManager.h"


// Sets default values
AAIRoamArea::AAIRoamArea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	bAlwaysRelevant = true;
	SetReplicateMovement(false);

	RoamingBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("RoamingBounds"));
	RootComponent = RoamingBounds;
	RoamingBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RoamingBounds->SetGenerateOverlapEvents(false);
	RoamingBounds->SetBoxExtent(FVector(500.0f, 500.0f, 150.0f));
}

// Called when the game starts or when spawned
void AAIRoamArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIRoamArea::Tick(float DeltaTime)
{
	for (FRoamingAIData& Element : CustomersRoaming)
	{
		if (!IsValid(Element.Customer) || Element.bShouldBeRemoved)
		{
			RemoveRoamingCustomer(Element.Customer);
			continue;
		}

		Element.RoamingTime -= DeltaTime;	
		if (Element.RoamingTime <= 0)
		{
			Element.Customer->GoapComp->NotifyInteractionReceived();
			RemoveRoamingCustomer(Element.Customer);
		}
	}
	
	Super::Tick(DeltaTime);
}

void AAIRoamArea::RequestAIRoam(ACustomer* Agent, float RoamingTime)
{
	if (!HasAuthority())
	{
		Server_RequestAIRoam(Agent, RoamingTime);
		return;
	}

	if (!IsValid(Agent)) return;
	
	UE_LOG(LogTemp, Error, TEXT("RequestAIRoam: %s is now roaming for %.2f seconds"), *Agent->GetName(), RoamingTime);
	
	CustomersRoaming.Add(FRoamingAIData{Agent, RoamingTime});
	
	Agent->MoveToTargetPosition(GetPointInZone());

	if (AQueueController* QueueController = Cast<AQueueController>(Agent->GetController()))
	{
		QueueController->MoveCompleted.AddUniqueDynamic(this, &AAIRoamArea::HandleCustomerMoveCompleted);
	}

	if (UGoapComponent* GoapComp = Agent->GoapComp)
	{
		GoapComp->OnAnyActionCompleted.AddUniqueDynamic(this, &AAIRoamArea::HandleCustomerGoapActionCompleted);
		GoapComp->OnAnyActionFailed.AddUniqueDynamic(this, &AAIRoamArea::HandleCustomerGoapActionFailed);
	}
}

void AAIRoamArea::Server_RequestAIRoam_Implementation(ACustomer* Agent, float RoamingTime)
{
	RequestAIRoam(Agent, RoamingTime);
}

void AAIRoamArea::HandleCustomerMoveCompleted(ACustomer* Agent)
{
	if (!IsValid(Agent))
	{
		return;
	}

	TWeakObjectPtr<ACustomer> WeakCustomer(Agent);
	FTimerDelegate DelayedMoveDelegate;
	DelayedMoveDelegate.BindLambda([this, WeakCustomer]()
	{
		if (WeakCustomer.IsValid())
		{
			WeakCustomer->MoveToTargetPosition(GetPointInZone());
		}
	});

	const float DelaySeconds = bUseRandomMoveDelay
		? FMath::FRandRange(RandomMoveDelayMinSeconds, FMath::Max(RandomMoveDelayMinSeconds, RandomMoveDelayMaxSeconds))
		: FixedMoveDelaySeconds;

	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(DelayHandle, DelayedMoveDelegate, DelaySeconds, false);
}

void AAIRoamArea::HandleCustomerGoapActionCompleted(const FGameplayTagContainer& EventTags, AActor* Agent)
{
	if (ACustomer* Customer = Cast<ACustomer>(Agent))
	{
		RemoveRoamingCustomer(Customer);
	}
}

void AAIRoamArea::HandleCustomerGoapActionFailed(const FGameplayTagContainer& EventTags, AActor* Agent)
{
	if (ACustomer* Customer = Cast<ACustomer>(Agent))
	{
		RemoveRoamingCustomer(Customer);
	}
}

void AAIRoamArea::UnbindCustomerMoveCompleted(ACustomer* Customer)
{
	if (AQueueController* QueueController = Customer ? Cast<AQueueController>(Customer->GetController()) : nullptr)
	{
		QueueController->MoveCompleted.RemoveDynamic(this, &AAIRoamArea::HandleCustomerMoveCompleted);
	}
}

void AAIRoamArea::UnbindCustomerGoapDelegates(ACustomer* Customer)
{
	if (UGoapComponent* GoapComp = Customer ? Customer->GoapComp : nullptr)
	{
		GoapComp->OnAnyActionCompleted.RemoveDynamic(this, &AAIRoamArea::HandleCustomerGoapActionCompleted);
		GoapComp->OnAnyActionFailed.RemoveDynamic(this, &AAIRoamArea::HandleCustomerGoapActionFailed);
	}
}

void AAIRoamArea::RemoveRoamingCustomer(ACustomer* Customer)
{
	if (!IsValid(Customer))
	{
		return;
	}

	for (int32 Index = CustomersRoaming.Num() - 1; Index >= 0; --Index)
	{
		if (CustomersRoaming[Index].Customer != Customer)
		{
			continue;
		}

		UnbindCustomerMoveCompleted(Customer);
		UnbindCustomerGoapDelegates(Customer);
		CustomersRoaming.RemoveAtSwap(Index);
		break;
	}
}

FVector AAIRoamArea::GetPointInZone() const
{
	if (!RoamingBounds)
	{
		return GetActorLocation();
	}

	const FVector Extent = RoamingBounds->GetScaledBoxExtent();
	const FVector Origin = RoamingBounds->GetComponentLocation();
	const FVector RandomPoint = Origin + FVector(
		FMath::FRandRange(-Extent.X, Extent.X),
		FMath::FRandRange(-Extent.Y, Extent.Y),
		FMath::FRandRange(-Extent.Z, Extent.Z));

	if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation ProjectedLocation;
		if (NavSystem->ProjectPointToNavigation(RandomPoint, ProjectedLocation, FVector(100.0f, 100.0f, 300.0f)))
		{
			return ProjectedLocation.Location;
		}
	}

	return RandomPoint;
}

