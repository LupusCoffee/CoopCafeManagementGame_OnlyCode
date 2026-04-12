// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackSplineComponent.h"

#include "Vehicle.generated.h"

class AVehicleManager;

UCLASS()
class COFFEESHOPGAME_API AVehicle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVehicle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Activate(UTrackSplineComponent* InFollowingPath);
	void Deactivate();

	virtual void Update(float DeltaTime);

	bool HasFinished();

	void SetManager(AVehicleManager* InManager);
	UStaticMeshComponent* GetMeshComp() { return MeshComp; };

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	USceneComponent* SceneRoot;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* MeshComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UTrackSplineComponent* FollowingTrack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float TravelledDistance;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int NextPointIndex;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float DistanceAtNextPoint;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EForwardOrder ForwardOrder = EForwardOrder::Ascending;

	UPROPERTY()
	AVehicleManager* VehicleManager;
};
