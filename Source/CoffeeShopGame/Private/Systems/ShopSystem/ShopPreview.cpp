// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/ShopSystem/ShopPreview.h"
#include "Systems/ShopSystem/ShopManager.h"

// Sets default values
AShopPreview::AShopPreview()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AShopPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShopPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

