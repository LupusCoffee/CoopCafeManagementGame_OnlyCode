// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopItemPreview.h"

// Sets default values
AShopItemPreview::AShopItemPreview()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetMobility(EComponentMobility::Movable);
}

// Called when the game starts or when spawned
void AShopItemPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShopItemPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShopItemPreview::SetMesh(UStaticMeshComponent* NewMesh)
{
	Mesh = NewMesh;
}

