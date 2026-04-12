// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Path.generated.h"

class UTrackSplineComponent;

UCLASS()
class APathMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void AddMainSpline();
#endif

	TRACKSYSTEM_API void IncreaseSubSpline();
	int GetSubSplineCount() { return NumOfSubSplines; }

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	USceneComponent* SceneRoot;

	UPROPERTY()
	int NumOfMainSplines;

	UPROPERTY()
	int NumOfSubSplines;
};
