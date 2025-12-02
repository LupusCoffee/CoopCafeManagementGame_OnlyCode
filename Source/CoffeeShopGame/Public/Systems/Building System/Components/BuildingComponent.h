// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/Building System/Actors/PreviewActor.h"

#include "BuildingComponent.generated.h"

class ULookTraceSubsystem;
class AFurniture;
class UBuildingSubsystem;
class AGridVisualizer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COFFEESHOPGAME_API UBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuildingComponent();

protected: 
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetBuildingPreviewActor(AFurniture* InPreviewActor);
	UFUNCTION()
	void SetPreviewMesh();
	bool ResetBuildingPreview(FVector LookAtLocation, FVector& OutPlaceLocation);
	AFurniture* GetBuildingPreviewActor();

	UFUNCTION(Server, Reliable)
	void SetOnGrid(bool IsOnGrid);

private:
	UPROPERTY()
	APawn* Player; 

	UPROPERTY()
	AGridVisualizer* GridVisualizer;

	UPROPERTY(ReplicatedUsing = SetPreviewMesh)
	AFurniture* Furniture;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APreviewActor> PreviewActorClass;

	UPROPERTY()
	APreviewActor* PreviewActor;

	UPROPERTY()
	ULookTraceSubsystem* TraceSubsystem;

	//Variables --> General
	UPROPERTY()
	AController* Controller = nullptr;

	UPROPERTY()
	UBuildingSubsystem* BuildingSubsystem;

	bool bOnGrid = false;

	UPROPERTY()
	FVector PlaceLocation;
};
