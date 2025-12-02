// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/BasicAIController.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GameFramework/Character.h"
#include "Customer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateCustomerActiveStatus, ACustomer*, customer);

UCLASS()
class COFFEESHOPGAME_API ACustomer : public ACharacter, public IInteractable
{
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	ABasicAIController* GetBasicAIController();

	UFUNCTION(BlueprintCallable)
	void SetBasicAIController(ABasicAIController* _controller);
	
	 
	
private:
	GENERATED_BODY()

public:
	virtual auto Hover_Implementation(FInteractionContext Context) -> void override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateCustomerActiveStatus OnUpdateCustomerActiveStatus;

	UFUNCTION(BlueprintCallable)
	ABasicAIController* GetAIController();
	
	UFUNCTION(BlueprintCallable)
	virtual void Enable();

	UFUNCTION(BlueprintCallable)
	virtual void Disable();

	UFUNCTION(BlueprintCallable)
	bool IsEnabled();
	
private:
	UPROPERTY()
	ABasicAIController* AIController; //Will be set in the controller OnPossess()

	UPROPERTY()
	bool Enabled = false;

};
