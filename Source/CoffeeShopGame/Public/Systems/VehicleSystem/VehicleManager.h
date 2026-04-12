// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeStructs.h"
#include <TrackSplineComponent.h>
#include "VehicleManager.generated.h"

class AVehicle;
class APathMap;
class UTimeSubsystem;
struct FTime;

USTRUCT(BlueprintType)
struct FTimeEvent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTime Time;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Efficiency;
};

UCLASS()
class COFFEESHOPGAME_API AVehicleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVehicleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DeactivateCar(AVehicle* Car);

private:
	UPROPERTY()
	TArray<AVehicle*> VehiclePool;

	UPROPERTY()
	TArray<AVehicle*> ActiveVehicles;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	APathMap* PathMap;
	UPROPERTY()
	TArray<UTrackSplineComponent*> AvailablePaths;

	TArray<int> RandIndex;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<AVehicle> VehicleClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<UStaticMesh*> VehicleModels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int PoolSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int SpawnAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float SpawnDelay;
	float Timer = 10000;
	float Efficiency = 1;

	UPROPERTY()
	UTimeSubsystem* TimeSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<FTimeEvent> Events;

	int UpcomingEvent = -1;
	bool FinishedEvents = false;
	int CurrentDay;
};
