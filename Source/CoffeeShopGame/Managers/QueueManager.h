// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QueueManager.generated.h"



UCLASS()
class COFFEESHOPGAME_API AQueueManager : public AActor
{
private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQueueManager();

	UFUNCTION(BlueprintCallable)
	void GenerateQueuePositions(int _amountOfPositionsToGenerate);

	UFUNCTION(BlueprintCallable)
	bool CanAddCustomer();

	UFUNCTION(BlueprintCallable)
	void CustomerLeavesQueue(ACharacter* customer);

	UFUNCTION(BlueprintCallable)
	void AddCustomer(ACharacter* customer, FVector &outLocationToMoveTo);
	
	UFUNCTION(BlueprintCallable)
	void GetQueuePositionOfCustomer(ACharacter* customer, FVector &outLocationToMoveTo);
	
	UFUNCTION(BlueprintCallable)
	void RelocateAllCustomers();

	UFUNCTION(BlueprintCallable)
	ACharacter* GetFirstInQueue();
	
	UPROPERTY(EditAnywhere)
	float DistanceBetweenQueuePositions = 200.0f;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> QueuePositions;

	UPROPERTY(VisibleAnywhere)
	TArray<ACharacter*> CustomerList;

	UPROPERTY(EditAnywhere)
	AActor* FrontOfQueuePosition;

protected:
	virtual void BeginPlay() override;
};
