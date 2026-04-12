// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCShopSystem/Public/ItemSpawnPoint.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemSpawnPoint::AItemSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Billboard = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	Billboard->SetupAttachment(RootComponent);

	Billboard->bIsScreenSizeScaled = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AItemSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItemSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemSpawnPoint::SpawnItem_Implementation()
{

	if (!CurrentItemPurchased && SpawnedItemActor)
	{
		SpawnedItemActor->Destroy();
	}
	
	SpawnPointLocation = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	FVector ItemSpawnLocation = FVector(SpawnPointLocation.X, SpawnPointLocation.Y, SpawnPointLocation.Z + ItemHeightOffSet);
	
	AActor* NewSpawnedItemActor = GetWorld()->SpawnActor<AActor>(CurrentItem, ItemSpawnLocation, Rotation);
	SpawnedItemActor = NewSpawnedItemActor;
	CurrentItemPurchased = false;
	ItemRestocked();
}

void AItemSpawnPoint::SetCurrentItem(TSubclassOf<AActor> Item)
{
	CurrentItem = Item;
}

void AItemSpawnPoint::SetItemPrice(float Value)
{
	ItemPrice = Value;
}

AActor* AItemSpawnPoint::BuyCurrentItem()
{
	CurrentItemPurchased = true;
	OnItemPurchased.Broadcast(ItemPrice);
	AActor* ReturnItem = SpawnedItemActor;
	
	return ReturnItem;
}

void AItemSpawnPoint::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemSpawnPoint, CurrentItem);
}

