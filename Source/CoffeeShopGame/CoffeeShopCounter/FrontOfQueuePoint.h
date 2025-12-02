// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FrontOfQueuePoint.generated.h"

UCLASS()
class COFFEESHOPGAME_API AFrontOfQueuePoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFrontOfQueuePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
