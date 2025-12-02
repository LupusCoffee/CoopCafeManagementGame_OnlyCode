// Fill out your copyright notice in the Description page of Project Settings.


#include "CoffeeShopTable.h"


void ACoffeeShopTable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Sets default values
ACoffeeShopTable::ACoffeeShopTable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	TableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairMesh"));
	RootComponent = TableMesh;
}
