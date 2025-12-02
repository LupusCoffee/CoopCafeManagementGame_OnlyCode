// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Customer/Customer.h"
#include "GameFramework/Actor.h"
#include "CustomerManager.generated.h"

UCLASS()
class COFFEESHOPGAME_API ACustomerManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACustomerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//Spawning
	UFUNCTION(Server, Reliable, BlueprintCallable , Category="Spawning")
	void SpawnCustomerServer(FVector SpawnLocation);
	
	UFUNCTION(Category="Spawning")
	ACustomer* SpawnCustomer(FVector SpawnLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<ACustomer> ActorToSpawn;

	UFUNCTION(BlueprintCallable, Category="Spawning")
	void ActivateCustomers(int n);
private:

public:
	//Management
	UFUNCTION(BlueprintCallable)
	bool AddCustomer(ACustomer* customer);
	
	UFUNCTION(BlueprintCallable)
	TArray<ACustomer*> GetAllCustomers();

	UFUNCTION(BlueprintCallable)
	void UpdateCustomerStatus(ACustomer* customer);

private:
	UPROPERTY(VisibleAnywhere)
	TArray<ACustomer*> AllCustomers;

	UPROPERTY(VisibleAnywhere)
	TArray<ACustomer*> InactiveCustomers;

	UPROPERTY(VisibleAnywhere)
	TArray<ACustomer*> ActiveCustomers;

public:
	UFUNCTION(BlueprintCallable)
	void ForceCustomersThirsty(int customersNumber);
};
