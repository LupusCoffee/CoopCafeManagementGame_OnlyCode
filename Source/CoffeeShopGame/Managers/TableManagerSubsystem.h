// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TableManagerSubsystem.generated.h"

class ATable;
class AChair;

/**
 * Tracks chair reservations for the seating system
 */
USTRUCT(BlueprintType)
struct FChairReservation
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	AChair* Chair = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* ReservedBy = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	float ReservationTime = 0.f;
	
	FChairReservation() = default;
	
	FChairReservation(AChair* InChair, AActor* InReservedBy, float InTime)
		: Chair(InChair), ReservedBy(InReservedBy), ReservationTime(InTime)
	{
	}
};

/**
 * Manages table and chair availability and reservations.
 * AI agents request chairs through this system.
 */
UCLASS()
class COFFEESHOPGAME_API UTableManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	/**
	 * Request any available chair. The chair will be reserved for the requester.
	 * @param Requester - The actor requesting the chair
	 * @param bRandom - If true, returns a random available chair instead of first found
	 * @return A reserved chair, or nullptr if none available
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	AChair* RequestAvailableChair(AActor* Requester, bool bRandom = false);
	
	/**
	 * Request a chair at a specific table. The chair will be reserved for the requester.
	 * @param Table - The table to request a chair from
	 * @param Requester - The actor requesting the chair
	 * @return A reserved chair at the table, or nullptr if none available
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	AChair* RequestChairAtTable(ATable* Table, AActor* Requester);
	
	/**
	 * Reserve a specific chair for an actor.
	 * @param Chair - The chair to reserve
	 * @param Requester - The actor reserving the chair
	 * @return True if reservation successful, false if chair already reserved/occupied
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	bool ReserveChair(AChair* Chair, AActor* Requester);
	
	/**
	 * Confirm a reservation when the AI actually sits down.
	 * Converts a reservation into actual occupancy.
	 * @param Chair - The chair to confirm
	 * @param Requester - The actor confirming (must match reservation)
	 * @return True if confirmed successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	bool ConfirmChairReservation(AChair* Chair, AActor* Requester);
	
	/**
	 * Release a chair (either reservation or actual occupancy).
	 * @param Chair - The chair to release
	 * @param Requester - The actor releasing it (must match current user/reservation)
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	void ReleaseChair(AChair* Chair, AActor* Requester);
	
	/**
	 * Check if a chair is available (not reserved and not occupied).
	 * @param Chair - The chair to check
	 * @return True if available
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	bool IsChairAvailable(const AChair* Chair) const;

	/**
	 * Check if there is at least one available chair across all registered tables.
	 * @return True if any chair is available, false if all chairs are occupied/reserved
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	bool HasAnyAvailableChair() const;
	
	/**
	 * Get who has reserved a chair, if anyone.
	 * @param Chair - The chair to check
	 * @return The actor who reserved it, or nullptr if not reserved
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	AActor* GetChairReservation(const AChair* Chair) const;
	
	/**
	 * Register a table with the manager.
	 * @param Table - The table to register
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	void RegisterTable(ATable* Table);
	
	/**
	 * Unregister a table from the manager.
	 * @param Table - The table to unregister
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	void UnregisterTable(ATable* Table);
	
	/**
	 * Get a table by its ID.
	 * @param TableId - The ID to search for
	 * @return The table with that ID, or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	ATable* GetTableById(int32 TableId) const;
	
	/**
	 * Get all registered tables.
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	TArray<ATable*> GetAllTables() const { return Tables; }
	
	/**
	 * Register a chair that doesn't have a table yet.
	 * Will be notified when new tables are added.
	 * @param Chair - The chair to register
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	void RegisterUnassignedChair(AChair* Chair);
	
	/**
	 * Unregister a chair from the unassigned list (called when chair gets assigned to a table).
	 * @param Chair - The chair to unregister
	 */
	UFUNCTION(BlueprintCallable, Category = "Table Manager")
	void UnregisterUnassignedChair(AChair* Chair);
	
	UFUNCTION(BlueprintCallable, Category = "Table Payment")
	void SendPayment(TSubclassOf<AActor> MoneyActorClass, int TableID);
	
protected:
	/** All registered tables */
	UPROPERTY()
	TArray<ATable*> Tables;
	
	/** Chairs that don't have a table assignment yet */
	UPROPERTY()
	TArray<AChair*> UnassignedChairs;
	
	/** Active chair reservations */
	UPROPERTY()
	TMap<AChair*, FChairReservation> ChairReservations;
	
	/** How long a reservation lasts before timing out (seconds) */
	UPROPERTY(EditDefaultsOnly, Category = "Table Manager")
	float ReservationTimeoutDuration = 30.f;
	
	/** How often to check for expired reservations (seconds) */
	UPROPERTY(EditDefaultsOnly, Category = "Table Manager")
	float CleanupCheckInterval = 5.f;
	
	/** Timer handle for cleanup */
	FTimerHandle CleanupTimerHandle;
	
	/** Clean up expired reservations */
	UFUNCTION()
	void CleanupExpiredReservations();
	
	/** Internal helper to reserve a chair */
	bool ReserveChairInternal(AChair* Chair, AActor* Requester);
};
