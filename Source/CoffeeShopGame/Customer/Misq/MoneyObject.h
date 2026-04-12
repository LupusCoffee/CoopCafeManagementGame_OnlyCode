// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GameFramework/Actor.h"
#include "MoneyObject.generated.h"

UCLASS()
class COFFEESHOPGAME_API AMoneyObject : public AActor, public IInteractable
{
public:
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual bool Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	virtual bool Server_TickInteraction_Implementation(EActionId ActionId, FPlayerContext Context, float DeltaTime) override;
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMoneyObject();

	UFUNCTION(BlueprintCallable)
	void SetAmount(float InAmount);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAmount();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	float Amount;
};
