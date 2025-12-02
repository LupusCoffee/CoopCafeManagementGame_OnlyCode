// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomerManager.h"
#include "UCustomerAISubsystem.generated.h"

class AQueueManager;
/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API UCustomerAISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UCustomerAISubsystem();
	// Called when the subsystem is created
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Called when the subsystem is destroyed
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	bool AssignCustomerManager(ACustomerManager* _customerManager);
	
	UFUNCTION(BlueprintCallable)
	ACustomerManager* GetCustomerManager();

private:
	UPROPERTY()
	ACustomerManager* CustomerManager;
};
