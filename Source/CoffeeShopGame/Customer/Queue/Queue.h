// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "Queue.generated.h"

class ACustomer;
class USplineComponent;

UCLASS()
class COFFEESHOPGAME_API AQueue : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQueue();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	bool AddCustomer(ACustomer* customer);

	UFUNCTION(BlueprintCallable)
	void CustomerLeavesQueue(ACustomer* customer);

	UFUNCTION(BlueprintCallable)
	void GenerateQueuePositions();
	
	UFUNCTION(BlueprintCallable)
	int GetPositionInQueue(ACustomer* customer) const;

	UPROPERTY(EditAnywhere)
	float DistanceBetweenQueuePositions = 200.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int NumQueuePositions = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* QueueSpline;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<FVector> QueuePositions;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<ACustomer*> CustomerList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ChairMesh;
	
private:
	UFUNCTION(Server, Reliable)
	void Server_AddCustomer(ACustomer* Customer);

	UFUNCTION(Server, Reliable)
	void Server_CustomerLeavesQueue(ACustomer* Customer);

	bool AddCustomerInternal(ACustomer* Customer);
	void CustomerLeavesQueueInternal(ACustomer* Customer);

	UFUNCTION(BlueprintCallable)
	bool CanAddCustomer();

	UFUNCTION(BlueprintCallable)
	void RelocateAllCustomers();
};
