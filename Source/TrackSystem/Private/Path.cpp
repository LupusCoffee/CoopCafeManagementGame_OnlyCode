// Fill out your copyright notice in the Description page of Project Settings.


#include "Path.h"
#include "TrackSplineComponent.h"

// Sets default values
APathMap::APathMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
}

// Called when the game starts or when spawned
void APathMap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APathMap::AddMainSpline()
{
	UTrackSplineComponent* NewSpline = NewObject<UTrackSplineComponent>(this, *FString("MainSpline" + FString::FromInt(NumOfMainSplines)));

	AddInstanceComponent(NewSpline);
	NewSpline->RegisterComponent();
	NewSpline->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	NewSpline->SetIsMainSpline(true);

	NumOfMainSplines++;
}

void APathMap::IncreaseSubSpline()
{
	NumOfSubSplines++;
}