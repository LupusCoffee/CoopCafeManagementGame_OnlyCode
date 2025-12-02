// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Customer/Enums/PointOfInterestType.h"
#include "GameFramework/Actor.h"
#include "AIPointOfInterest.generated.h"

UCLASS()
class COFFEESHOPGAME_API AAIPointOfInterest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIPointOfInterest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	EPointOfInterestType PointType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
