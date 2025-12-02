// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopChair.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopTable.generated.h"

UCLASS()
class COFFEESHOPGAME_API ACoffeeShopTable : public AActor
{
protected:
	virtual void BeginPlay() override;

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoffeeShopTable();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* TableMesh;

	UPROPERTY(editAnywhere, BlueprintReadWrite)
	TArray<ACoffeeShopChair*> TableChairs;
};
