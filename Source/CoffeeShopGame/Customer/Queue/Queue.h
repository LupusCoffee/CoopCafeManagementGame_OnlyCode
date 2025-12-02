// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Queue.generated.h"

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

	UFUNCTION(BlueprintCallable)
	bool AddCustomer(APawn* customer);

	UFUNCTION(BlueprintCallable)
	void CustomerLeavesQueue(APawn* customer);

	UFUNCTION(BlueprintCallable)
	void GenerateQueuePositions(int _amountOfPositionsToGenerate);

	UPROPERTY(EditAnywhere)
	float DistanceBetweenQueuePositions = 200.0f;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> QueuePositions;
	
	UPROPERTY(VisibleAnywhere)
	TArray<APawn*> CustomerList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ChairMesh;
	
private:
	UFUNCTION(BlueprintCallable)
	bool CanAddCustomer();

	UFUNCTION(BlueprintCallable)
	void RelocateAllCustomers();
};
