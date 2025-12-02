// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/Building System/Actors/PreviewActor.h"

// Sets default values
APreviewActor::APreviewActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
}

void APreviewActor::SetMesh(UStaticMesh* Mesh)
{
	MeshComp->SetStaticMesh(Mesh);
}

void APreviewActor::SetCanPlacePreview(bool CanPlace)
{
	if (CanPlace)
		MeshComp->SetVectorParameterValueOnMaterials("PreviewColor", FVector(0, 1, 0));
	else
		MeshComp->SetVectorParameterValueOnMaterials("PreviewColor", FVector(1, 0, 0));
}

// Called when the game starts or when spawned
void APreviewActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

