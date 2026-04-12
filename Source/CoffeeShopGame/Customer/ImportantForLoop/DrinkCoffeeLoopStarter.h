// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOAPSystem/AI/Interface/Interractable.h"
#include "DrinkCoffeeLoopStarter.generated.h"

UCLASS()
class COFFEESHOPGAME_API ADrinkCoffeeLoopStarter : public AActor, public IGOAPInteractable
{
public:
	virtual void Interact_Implementation(AActor* Agent) override;

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADrinkCoffeeLoopStarter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
