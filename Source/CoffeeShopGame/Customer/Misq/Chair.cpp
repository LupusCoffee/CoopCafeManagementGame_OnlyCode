// Fill out your copyright notice in the Description page of Project Settings.

#include "Chair.h"
#include "Table.h"
#include "CoffeeShopGame/Customer/Customer.h"
#include "CoffeeShopGame/Managers/TableManagerSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(CoffeeNet, Log, All);

AChair::AChair()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ChairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairMesh"));
	RootComponent = ChairMesh;

	ChairMovePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Chair move point"));
	ChairMovePoint->SetupAttachment(ChairMesh); // attach to mesh, not RootComponent
	ChairMovePoint->SetCanEverAffectNavigation(false);
}

void AChair::BeginPlay()
{
	Super::BeginPlay();
	
	// Try to find a table on spawn
	ReevaluateTableAssignment();
}

//------------------------------------------------------------
// TABLE ASSOCIATION
//------------------------------------------------------------

void AChair::ReevaluateTableAssignment()
{
	if (UTableManagerSubsystem* TableManager = GetWorld()->GetSubsystem<UTableManagerSubsystem>())
	{
		// If we already have a table, don't reassign unless we want to allow that
		if (ParentTable)
		{
			UE_LOG(CoffeeNet, Verbose, TEXT("Chair: Already assigned to table %d, skipping reassignment"), 
				ParentTable->GetTableId());
			return;
		}
		
		ATable* ClosestTable = nullptr;
		
		// Find the closest table to this chair
		for (auto* table : TableManager->GetAllTables())
		{
			if (ClosestTable == nullptr)
			{
				ClosestTable = table;
				continue;
			}
			
			if (table == ClosestTable) continue;
			
			float distanceToTable = GetDistanceTo(table);
			float distanceToClosestTable = GetDistanceTo(ClosestTable);
			
			if (distanceToTable < distanceToClosestTable)
				ClosestTable = table;
		}
		
		// Only add to table if we found one
		if (ClosestTable)
		{
			ClosestTable->AddChair(this);
			// Remove from unassigned list now that we have a table
			TableManager->UnregisterUnassignedChair(this);
			UE_LOG(CoffeeNet, Log, TEXT("Chair: Assigned to table %d"), ClosestTable->GetTableId());
		}
		else
		{
			// Register as unassigned so we get notified when tables spawn
			TableManager->RegisterUnassignedChair(this);
			UE_LOG(CoffeeNet, Warning, TEXT("Chair: No tables found, registered as unassigned"));
		}
	}
}

void AChair::AIInteract_Implementation(APawn* Agent)
{
	if (!HasAuthority())
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("[ServerOnly][NetAuth][Chair] AIInteract ignored on client for %s"), *GetName());
		return;
	}

	if (auto Cust = Cast<ACustomer>(Agent))
	{
		if (Cust->bIsSitting)
		{
			Cust->bIsSitting = false;
		}
		else
		{
			Cust->bIsSitting = true;
		}
	}
	IInteractable::AIInteract_Implementation(Agent);
}

//------------------------------------------------------------
// OCCUPANCY API
//------------------------------------------------------------

bool AChair::IsTaken() const
{
	return CurrentUser.IsValid();
}

AActor* AChair::GetCurrentUser() const
{
	return CurrentUser.Get();
}

void AChair::SetCurrentUser(AActor* NewUser)
{
	if (IsTaken())
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Chair: Attempted to set user on occupied chair"));
		return;
	}
	
	CurrentUser = NewUser;
	UE_LOG(CoffeeNet, Verbose, TEXT("Chair: User set to %s"), NewUser ? *NewUser->GetName() : TEXT("None"));
}

void AChair::LeaveChair()
{
	if (CurrentUser.IsValid())
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("Chair: User %s left chair"), *CurrentUser->GetName());
	}
	CurrentUser.Reset();
}

FVector AChair::GetSitPosition() const
{
	return GetActorLocation() + SitPositionOffset;
}

FRotator AChair::GetSitRotation() const
{
	return bUseChairRotation ? GetActorRotation() : SitRotationOverride;
}

//------------------------------------------------------------
// IInteractable Interface Implementation
//------------------------------------------------------------



