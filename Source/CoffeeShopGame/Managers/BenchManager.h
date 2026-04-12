// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOAPSystem/AI/Interface/Interractable.h"
#include "BenchManager.generated.h"

UCLASS()
class COFFEESHOPGAME_API ABenchManager : public AActor, public IGOAPInteractable
{
private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABenchManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// List of all benches found with the "Bench" tag
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bench")
	TArray<AActor*> Benches;

public:
	// Function to get the closest bench to a given actor
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Bench")
	AActor* GetClosestBench(AActor* TargetActor) const;
	
	virtual void Interact_Implementation(AActor* Agent) override;
};

