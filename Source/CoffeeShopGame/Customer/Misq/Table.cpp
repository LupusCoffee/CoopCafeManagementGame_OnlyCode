// Fill out your copyright notice in the Description page of Project Settings.

#include "Table.h"
#include "Chair.h"
#include "CoffeeShopGame/Customer/Customer.h"
#include "CoffeeShopGame/Managers/TableManagerSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(CoffeeNet, Log, All);

void ATable::AIInteract_Implementation(APawn* Agent)
{
	auto cust = Cast<ACustomer>(Agent);
	
	if (!cust)
		return;
	
	//TODO: Call paymentManager and place money on the table depending on how much the AI wants to pay
	
	IInteractable::AIInteract_Implementation(Agent);
}

ATable::ATable()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Create root scene component
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	
	// Create static mesh component for table visual
	TableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMesh"));
	TableMesh->SetupAttachment(RootComponent);
	TableMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TableMesh->SetCollisionResponseToAllChannels(ECR_Block);
	
	// Create static mesh component for position to spawn money
	MoneyPosition = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoneyPosition"));
	MoneyPosition->SetupAttachment(RootComponent);
	
	// Create collision component for detecting items on table surface
	TableSurfaceCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TableSurfaceCollision"));
	TableSurfaceCollision->SetupAttachment(RootComponent);
	
	// Default size - can be changed in Blueprint!
	TableSurfaceCollision->SetBoxExtent(FVector(100.f, 100.f, 10.f));
	
	// Position above the root by default - adjust in Blueprint!
	TableSurfaceCollision->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	
	// Configure collision
	TableSurfaceCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TableSurfaceCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TableSurfaceCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	TableSurfaceCollision->SetGenerateOverlapEvents(true);
}

void ATable::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind collision event
	if (TableSurfaceCollision)
	{
		TableSurfaceCollision->OnComponentBeginOverlap.AddDynamic(this, &ATable::OnTableSurfaceBeginOverlap);
		TableSurfaceCollision->OnComponentEndOverlap.AddDynamic(this, &ATable::OnTableSurfaceEndOverlap);
	}
	
	// Register with the TableManagerSubsystem
	if (UTableManagerSubsystem* TableManager = GetWorld()->GetSubsystem<UTableManagerSubsystem>())
	{
		TableManager->RegisterTable(this);
		UE_LOG(CoffeeNet, Log, TEXT("Table %d: Registered with TableManagerSubsystem"), TableId);
	}
}

//------------------------------------------------------------
// CHAIR MANAGEMENT
//------------------------------------------------------------

void ATable::AddChair(AChair* Chair)
{
	if (!Chair)
	{
		return;
	}
	
	if (!ChairsAtTable.Contains(Chair))
	{
		ChairsAtTable.Add(Chair);
		Chair->SetParentTable(this);
		UE_LOG(CoffeeNet, Verbose, TEXT("Table %d: Chair added (Total: %d)"), TableId, ChairsAtTable.Num());
	}
}

void ATable::RemoveChair(AChair* Chair)
{
	if (!Chair)
	{
		return;
	}
	
	if (ChairsAtTable.Contains(Chair))
	{
		ChairsAtTable.Remove(Chair);
		Chair->SetParentTable(nullptr);
		UE_LOG(CoffeeNet, Verbose, TEXT("Table %d: Chair removed (Total: %d)"), TableId, ChairsAtTable.Num());
	}
}

bool ATable::IsTableFull() const
{
	int32 OccupiedCount = 0;
	for (AChair* Chair : ChairsAtTable)
	{
		if (Chair && Chair->IsTaken())
		{
			OccupiedCount++;
		}
	}
	
	return OccupiedCount >= ChairsAtTable.Num();
}

int32 ATable::GetOccupiedChairCount() const
{
	int32 Count = 0;
	for (AChair* Chair : ChairsAtTable)
	{
		if (Chair && Chair->IsTaken())
		{
			Count++;
		}
	}
	return Count;
}

//------------------------------------------------------------
// EVENT NOTIFICATIONS
//------------------------------------------------------------

void ATable::OnTableSurfaceBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore if it's a chair or customer
	if (!OtherActor || OtherActor == this || OtherActor->IsA<AChair>())
	{
		return;
	}
	
	// Check if it's a customer/character - don't register them as items
	if (OtherActor->IsA<ACharacter>() || OtherActor->IsA<APawn>())
	{
		return;
	}
	
	// This is an item being placed on the table
	UE_LOG(CoffeeNet, Log, TEXT("Table %d: Item detected via collision: %s"), TableId, *OtherActor->GetName());
	
	// Add to items list if not already there
	if (!ItemsOnTable.Contains(OtherActor))
	{
		ItemsOnTable.Add(OtherActor);
	}
	
	// Broadcast to all seated customers
	TArray<AActor*> SeatedCustomers = GetSeatedCustomers();
	if (SeatedCustomers.Num() > 0)
	{
		UE_LOG(CoffeeNet, Log, TEXT("Table %d: Broadcasting item to %d seated customers"), 
			TableId, SeatedCustomers.Num());
	}
	
	// Fire the event - all seated customers can bind to this
	OnItemDetected.Broadcast(this, OtherActor);
}

void ATable::OnTableSurfaceEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Ignore if it's a chair or customer
	if (!OtherActor || OtherActor == this || OtherActor->IsA<AChair>())
	{
		return;
	}
	
	// Check if it's a customer/character - don't register them as items
	if (OtherActor->IsA<ACharacter>() || OtherActor->IsA<APawn>())
	{
		return;
	}
	
	// This is an item being removed from the table
	UE_LOG(CoffeeNet, Log, TEXT("Table %d: Item removed via collision: %s"), TableId, *OtherActor->GetName());
	
	// Remove from items list if it's there
	if (ItemsOnTable.Contains(OtherActor))
	{
		ItemsOnTable.Remove(OtherActor);
	}
	
	// Broadcast to all seated customers
	TArray<AActor*> SeatedCustomers = GetSeatedCustomers();
	if (SeatedCustomers.Num() > 0)
	{
		UE_LOG(CoffeeNet, Log, TEXT("Table %d: Broadcasting item removal to %d seated customers"), 
			TableId, SeatedCustomers.Num());
	}
	
	// Fire the event - all seated customers can bind to this
	OnItemRemoved.Broadcast(this, OtherActor);
}

void ATable::OnCustomerSeated(AActor* Customer, AChair* Chair)
{
	if (!Customer || !Chair)
	{
		return;
	}
	
	// Broadcast the event
	OnCustomerSeatedEvent.Broadcast(this, Customer, Chair);
	
	UE_LOG(CoffeeNet, Log, TEXT("Table %d: Customer %s seated"), TableId, *Customer->GetName());
}

TArray<AActor*> ATable::GetSeatedCustomers() const
{
	TArray<AActor*> SeatedCustomers;
	
	for (AChair* Chair : ChairsAtTable)
	{
		if (Chair && Chair->IsTaken())
		{
			if (AActor* Customer = Chair->GetCurrentUser())
			{
				SeatedCustomers.Add(Customer);
			}
		}
	}
	
	return SeatedCustomers;
}

