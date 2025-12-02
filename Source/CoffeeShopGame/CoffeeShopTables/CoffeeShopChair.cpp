// Fill out your copyright notice in the Description page of Project Settings.


#include "CoffeeShopChair.h"

#include "CoffeeShopTable.h"
#include "Kismet/GameplayStatics.h"


void ACoffeeShopChair::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoffeeShopTable::StaticClass(), FoundActors);

	FVector self = GetActorLocation();

	for (AActor* table : FoundActors)
	{
		float Distance = FVector::Dist(self, table->GetActorLocation());
		if (Distance < 500)
		{
			Cast<ACoffeeShopTable>(table)->TableChairs.Add(this);
			break;
		}
	}
}

// Sets default values
ACoffeeShopChair::ACoffeeShopChair(): ChairMesh(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ChairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairMesh"));

	SetRootComponent(ChairMesh);
}

bool ACoffeeShopChair::IsEmpty()
{
	return bIsEmpty;
}

void ACoffeeShopChair::AssignChair(bool fillStatus)
{
	bIsEmpty = fillStatus;
}


