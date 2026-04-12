// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "Table.generated.h"

class AChair;

/**
 * Delegate fired when a drink is placed on this table
 * @param Table - The table the drink was placed on
 * @param DrinkActor - The drink that was placed
 * @param TargetCustomer - The customer the drink is for (optional)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDrinkPlacedSignature, ATable*, Table, AActor*, DrinkActor, AActor*, TargetCustomer);

/**
 * Delegate fired when any item is detected on the table surface via collision
 * @param Table - The table the item was placed on
 * @param Item - The item that was detected
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemDetectedSignature, ATable*, Table, AActor*, Item);

/**
 * Delegate fired when an item is removed from the table surface via collision
 * @param Table - The table the item was removed from
 * @param Item - The item that was removed
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedSignature, ATable*, Table, AActor*, Item);

/**
 * Delegate fired when a customer sits at this table
 * @param Table - The table
 * @param Customer - The customer who sat down
 * @param Chair - The chair they sat in
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCustomerSeatedSignature, ATable*, Table, AActor*, Customer, AChair*, Chair);

/**
 * Represents a table in the restaurant.
 * Manages chairs and items on the table, and notifies listeners of events.
 */
UCLASS()
class COFFEESHOPGAME_API ATable : public AActor, public IInteractable
{
public:
	virtual void AIInteract_Implementation(APawn* Agent) override;

private:
	GENERATED_BODY()

public:
	ATable();

protected:
	virtual void BeginPlay() override;

public:
	//------------------------------------------------------------
	// TABLE IDENTIFICATION
	//------------------------------------------------------------
	
	/**
	 * Get this table's unique ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	int32 GetTableId() const { return TableId; }
	
	//------------------------------------------------------------
	// CHAIR MANAGEMENT
	//------------------------------------------------------------
	
	/**
	 * Register a chair with this table
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	void AddChair(AChair* Chair);
	
	/**
	 * Unregister a chair from this table
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	void RemoveChair(AChair* Chair);
	
	/**
	 * Get all chairs at this table
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	TArray<AChair*> GetChairsAtTable() const { return ChairsAtTable; }
	
	/**
	 * Get number of chairs at this table
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	int32 GetChairCount() const { return ChairsAtTable.Num(); }
	
	/**
	 * Check if all chairs are occupied
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	bool IsTableFull() const;
	
	/**
	 * Get number of occupied chairs
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	int32 GetOccupiedChairCount() const;
	
	//------------------------------------------------------------
	// ITEM MANAGEMENT (drinks, etc.)
	//------------------------------------------------------------
	
	/**
	 * Get all items currently on this table
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	TArray<AActor*> GetItemsOnTable() const { return ItemsOnTable; }
	
	//------------------------------------------------------------
	// EVENT NOTIFICATIONS
	//------------------------------------------------------------
	
	/**
	 * Called by Chair when a customer sits down
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	void OnCustomerSeated(AActor* Customer, AChair* Chair);
	
	/**
	 * Get all customers currently seated at this table
	 */
	UFUNCTION(BlueprintCallable, Category = "Table")
	TArray<AActor*> GetSeatedCustomers() const;
	
	/**
	 * Broadcast when a drink is placed on this table
	 */
	UPROPERTY(BlueprintAssignable, Category = "Table|Events")
	FOnDrinkPlacedSignature OnDrinkPlaced;
	
	/**
	 * Broadcast when any item is detected on the table surface (via collision)
	 * All seated customers will receive this notification
	 */
	UPROPERTY(BlueprintAssignable, Category = "Table|Events")
	FOnItemDetectedSignature OnItemDetected;
	
	/**
	 * Broadcast when an item is removed from the table surface (via collision)
	 * All seated customers will receive this notification
	 */
	UPROPERTY(BlueprintAssignable, Category = "Table|Events")
	FOnItemRemovedSignature OnItemRemoved;
	
	/**
	 * Broadcast when a customer sits at this table
	 */
	UPROPERTY(BlueprintAssignable, Category = "Table|Events")
	FOnCustomerSeatedSignature OnCustomerSeatedEvent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Components")
	UStaticMeshComponent* MoneyPosition;
	
protected:
	/** Static mesh for the table visual */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Components")
	UStaticMeshComponent* TableMesh;

	/** Collision box to detect items placed on table surface - Configure size and position in Blueprint! */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Components")
	UBoxComponent* TableSurfaceCollision;
	
	/** Callback when something enters the table surface */
	UFUNCTION()
	void OnTableSurfaceBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** Callback when something leaves the table surface */
	UFUNCTION()
	void OnTableSurfaceEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/** Unique identifier for this table */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Table")
	int32 TableId = 0;
	
	/** All chairs registered with this table */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Table")
	TArray<AChair*> ChairsAtTable;
	
	/** Items currently on this table (drinks, plates, etc.) */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Table")
	TArray<AActor*> ItemsOnTable;
};
