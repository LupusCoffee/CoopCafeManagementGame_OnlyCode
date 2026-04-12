// Fill out your copyright notice in the Description page of Project Settings.

#include "TableManagerSubsystem.h"
#include "CoffeeShopGame/Customer/Misq/Table.h"
#include "CoffeeShopGame/Customer/Misq/Chair.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(CoffeeNet, Log, All);

void UTableManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(CoffeeNet, Log, TEXT("TableManagerSubsystem initialized"));
}

void UTableManagerSubsystem::Deinitialize()
{
	// Clear cleanup timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CleanupTimerHandle);
	}
	
	Tables.Empty();
	ChairReservations.Empty();
	
	Super::Deinitialize();
}

void UTableManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// Start periodic cleanup of expired reservations
	InWorld.GetTimerManager().SetTimer(
		CleanupTimerHandle,
		this,
		&UTableManagerSubsystem::CleanupExpiredReservations,
		CleanupCheckInterval,
		true
	);
}

AChair* UTableManagerSubsystem::RequestAvailableChair(AActor* Requester, bool bRandom)
{
	if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("[ServerOnly][NetAuth][TableManagerSubsystem] RequestAvailableChair ignored on client"));
		return nullptr;
	}

	if (!Requester)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("TableManagerSubsystem: RequestAvailableChair called with null Requester"));
		return nullptr;
	}
	
	TArray<AChair*> AvailableChairs;
	
	// Gather all available chairs from all tables
	for (ATable* Table : Tables)
	{
		if (!Table) continue;
		
		TArray<AChair*> TableChairs = Table->GetChairsAtTable();
		for (AChair* Chair : TableChairs)
		{
			if (Chair && IsChairAvailable(Chair))
			{
				AvailableChairs.Add(Chair);
			}
		}
	}
	
	if (AvailableChairs.Num() == 0)
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: No available chairs found"));
		return nullptr;
	}
	
	// Select chair (random or first)
	AChair* SelectedChair = bRandom 
		? AvailableChairs[FMath::RandRange(0, AvailableChairs.Num() - 1)]
		: AvailableChairs[0];
	
	// Reserve it
	if (ReserveChairInternal(SelectedChair, Requester))
	{
		return SelectedChair;
	}
	
	return nullptr;
}

AChair* UTableManagerSubsystem::RequestChairAtTable(ATable* Table, AActor* Requester)
{
	if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("[ServerOnly][NetAuth][TableManagerSubsystem] RequestChairAtTable ignored on client"));
		return nullptr;
	}

	if (!Table || !Requester)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("TableManagerSubsystem: RequestChairAtTable called with null Table or Requester"));
		return nullptr;
	}
	
	// Get available chair from the specific table
	TArray<AChair*> TableChairs = Table->GetChairsAtTable();
	for (AChair* Chair : TableChairs)
	{
		if (Chair && IsChairAvailable(Chair))
		{
			if (ReserveChairInternal(Chair, Requester))
			{
				return Chair;
			}
		}
	}
	
	UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: No available chairs at table %d"), Table->GetTableId());
	return nullptr;
}

bool UTableManagerSubsystem::ReserveChair(AChair* Chair, AActor* Requester)
{
	if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("[ServerOnly][NetAuth][TableManagerSubsystem] ReserveChair ignored on client"));
		return false;
	}

	if (!Chair || !Requester)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("TableManagerSubsystem: ReserveChair called with null Chair or Requester"));
		return false;
	}
	
	return ReserveChairInternal(Chair, Requester);
}

bool UTableManagerSubsystem::ReserveChairInternal(AChair* Chair, AActor* Requester)
{
	// Check if already reserved
	if (ChairReservations.Contains(Chair))
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Chair already reserved by %s"), 
			*ChairReservations[Chair].ReservedBy->GetName());
		return false;
	}
	
	// Check if already occupied
	if (Chair->IsTaken())
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Chair already occupied"));
		return false;
	}
	
	// Create reservation
	float CurrentTime = GetWorld()->GetTimeSeconds();
	ChairReservations.Add(Chair, FChairReservation(Chair, Requester, CurrentTime));
	
	UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Chair reserved for %s"), *Requester->GetName());
	return true;
}

bool UTableManagerSubsystem::ConfirmChairReservation(AChair* Chair, AActor* Requester)
{
	if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("[ServerOnly][NetAuth][TableManagerSubsystem] ConfirmChairReservation ignored on client"));
		return false;
	}

	if (!Chair || !Requester)
	{
		return false;
	}
	
	// Check if this requester has the reservation
	const FChairReservation* Reservation = ChairReservations.Find(Chair);
	if (!Reservation || Reservation->ReservedBy != Requester)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("TableManagerSubsystem: Cannot confirm - no valid reservation for %s"), 
			*Requester->GetName());
		return false;
	}
	
	// Remove reservation and set actual user
	ChairReservations.Remove(Chair);
	Chair->SetCurrentUser(Requester);
	
	UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Reservation confirmed for %s"), *Requester->GetName());
	return true;
}

void UTableManagerSubsystem::ReleaseChair(AChair* Chair, AActor* Requester)
{
	if (!Chair || !Requester)
	{
		return;
	}
	
	// Check if has reservation
	if (ChairReservations.Contains(Chair))
	{
		const FChairReservation& Reservation = ChairReservations[Chair];
		if (Reservation.ReservedBy == Requester)
		{
			ChairReservations.Remove(Chair);
			UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Reservation released by %s"), *Requester->GetName());
			return;
		}
	}
	
	// Check if currently using
	if (Chair->GetCurrentUser() == Requester)
	{
		Chair->LeaveChair();
		UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Chair vacated by %s"), *Requester->GetName());
	}
}

bool UTableManagerSubsystem::IsChairAvailable(const AChair* Chair) const
{
	if (!Chair)
	{
		return false;
	}
	
	// Check if occupied
	if (Chair->IsTaken())
	{
		return false;
	}
	
	// Check if reserved
	if (ChairReservations.Contains(Chair))
	{
		return false;
	}
	
	return true;
}

bool UTableManagerSubsystem::HasAnyAvailableChair() const
{
	for (ATable* Table : Tables)
	{
		if (!Table)
		{
			continue;
		}

		const TArray<AChair*> TableChairs = Table->GetChairsAtTable();
		for (AChair* Chair : TableChairs)
		{
			if (Chair && IsChairAvailable(Chair))
			{
				return true;
			}
		}
	}

	return false;
}

AActor* UTableManagerSubsystem::GetChairReservation(const AChair* Chair) const
{
	if (!Chair)
	{
		return nullptr;
	}
	
	const FChairReservation* Reservation = ChairReservations.Find(Chair);
	return Reservation ? Reservation->ReservedBy : nullptr;
}

void UTableManagerSubsystem::RegisterTable(ATable* Table)
{
	if (!Table)
	{
		return;
	}
	
	if (Tables.Contains(Table))
	{
		return;
	}
	
	Tables.Add(Table);
	UE_LOG(CoffeeNet, Log, TEXT("TableManagerSubsystem: Registered table %d"), Table->GetTableId());
	
	// Notify all unassigned chairs to re-evaluate their table assignment
	if (UnassignedChairs.Num() > 0)
	{
		UE_LOG(CoffeeNet, Log, TEXT("TableManagerSubsystem: Notifying %d unassigned chairs about new table"), 
			UnassignedChairs.Num());
		
		// Create a copy since chairs will modify the array as they get assigned
		TArray<AChair*> ChairsToNotify = UnassignedChairs;
		
		for (AChair* Chair : ChairsToNotify)
		{
			if (Chair)
			{
				Chair->ReevaluateTableAssignment();
			}
		}
	}
}

void UTableManagerSubsystem::UnregisterTable(ATable* Table)
{
	if (!Table)
	{
		return;
	}
	
	// Clean up any reservations for chairs at this table
	TArray<AChair*> TableChairs = Table->GetChairsAtTable();
	for (AChair* Chair : TableChairs)
	{
		if (ChairReservations.Contains(Chair))
		{
			ChairReservations.Remove(Chair);
		}
	}
	
	Tables.Remove(Table);
	UE_LOG(CoffeeNet, Log, TEXT("TableManagerSubsystem: Unregistered table %d"), Table->GetTableId());
}

ATable* UTableManagerSubsystem::GetTableById(int32 TableId) const
{
	for (ATable* Table : Tables)
	{
		if (Table && Table->GetTableId() == TableId)
		{
			return Table;
		}
	}
	
	return nullptr;
}

void UTableManagerSubsystem::CleanupExpiredReservations()
{
	if (!GetWorld())
	{
		return;
	}
	
	float CurrentTime = GetWorld()->GetTimeSeconds();
	TArray<AChair*> ExpiredChairs;
	
	// Find expired reservations
	for (const TPair<AChair*, FChairReservation>& Pair : ChairReservations)
	{
		float ReservationAge = CurrentTime - Pair.Value.ReservationTime;
		if (ReservationAge >= ReservationTimeoutDuration)
		{
			ExpiredChairs.Add(Pair.Key);
			UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Reservation expired for %s (age: %.1fs)"), 
				*Pair.Value.ReservedBy->GetName(), ReservationAge);
		}
	}
	
	// Remove expired reservations
	for (AChair* Chair : ExpiredChairs)
	{
		ChairReservations.Remove(Chair);
	}
}

void UTableManagerSubsystem::RegisterUnassignedChair(AChair* Chair)
{
	if (!Chair)
	{
		return;
	}
	
	if (!UnassignedChairs.Contains(Chair))
	{
		UnassignedChairs.Add(Chair);
		UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Registered unassigned chair"));
	}
}

void UTableManagerSubsystem::UnregisterUnassignedChair(AChair* Chair)
{
	if (!Chair)
	{
		return;
	}
	
	if (UnassignedChairs.Contains(Chair))
	{
		UnassignedChairs.Remove(Chair);
		UE_LOG(CoffeeNet, Verbose, TEXT("TableManagerSubsystem: Unregistered chair from unassigned list"));
	}
}

void UTableManagerSubsystem::SendPayment(TSubclassOf<AActor> MoneyActorClass, int TableID)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	ATable* Table = GetTableById(TableID);
	if (!Table)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("SendPayment: No table found with ID %d"), TableID);
		return;
	}

	
	// Define spawn location/rotation
	FVector SpawnLocation = Table->MoneyPosition->GetComponentLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn the actor — replace AYourActorClass with your actual class
	AActor* SpawnedActor = World->SpawnActor<AActor>(
		MoneyActorClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (!SpawnedActor)
	{
		UE_LOG(CoffeeNet, Warning, TEXT("SendPayment: Failed to spawn actor"));
		return;
	}
}

