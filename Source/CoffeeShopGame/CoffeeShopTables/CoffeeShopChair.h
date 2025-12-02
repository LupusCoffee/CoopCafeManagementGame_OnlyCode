// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopChair.generated.h"

UCLASS()
class COFFEESHOPGAME_API ACoffeeShopChair : public AActor
{
protected:
	virtual void BeginPlay() override;

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoffeeShopChair();

	UFUNCTION(BlueprintCallable)
	bool IsEmpty();

	UFUNCTION(BlueprintCallable)
	void AssignChair(bool fillStatus);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ChairMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEmpty = false;
};
