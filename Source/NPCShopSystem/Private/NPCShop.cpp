// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCShopSystem/Public/NPCShop.h"


// Sets default values
ANPCShop::ANPCShop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ANPCShop::InnitialSetup()
{
	if (!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable is NULL in NPCShop"));
		return;
	}
	
	RowNames = DataTable->GetRowNames();

	for (FName name : RowNames)
	{
		FItemArrayWrapper ItemArray;

		FShopCatalog* Row = DataTable->FindRow<FShopCatalog>(name, TEXT("NPCShop: ItemArray"));
		

		if (Row && Row->PriceRanges.Num() > 0)
		{
			const FShopPriceRanges& PriceRange = Row->PriceRanges[Low];
			if (PriceRange.LowRange.Num() > 0)
			{
				for (FItemStruct ItemStruct : PriceRange.LowRange)
				{
					ItemArray.Items.Add(ItemStruct);
				}
			}
		}
		
		ActiveItemPool.Add(name, ItemArray);
	}
	
	RestockItems();

	SubscribeToEvents();
}


// Called when the game starts or when spawned
void ANPCShop::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ANPCShop::InnitialSetup, 3.0f, false);
}


void ANPCShop::SpawnRandomItem(AItemSpawnPoint* SpawnPoint)
{
	if (SpawnPoint == nullptr)
	{
		return;
	}



	FName RowName = SpawnPoint->ShopCategory.RowName;
	
	if (ActiveItemPool.Contains(RowName))
	{
		if (ActiveItemPool[RowName].Items.IsEmpty())
		{
			return;
		}
		int32 RandomIndex = FMath::RandRange(0, ActiveItemPool[RowName].Items.Num() - 1);
		SpawnPoint->CurrentItem = ActiveItemPool[RowName].Items[RandomIndex].Item;
		SpawnPoint->SetItemPrice(ActiveItemPool[RowName].Items[RandomIndex].Price);
		SpawnPoint->SpawnItem();
	}
}

void ANPCShop::SubscribeToEvents()
{
	if (RotatingItemSpawnPoints.Num() > 0)
	{
		for (int i = 0; i < RotatingItemSpawnPoints.Num(); i++)
		{
			if (RotatingItemSpawnPoints[i])
			{
				RotatingItemSpawnPoints[i]->OnItemPurchased.AddDynamic(this, &ANPCShop::IncreaseStoreFunds);
			}
		}
	}

	if (StaticItemSpawnPoints.Num() > 0)
	{
		for (int i = 0; i < StaticItemSpawnPoints.Num(); i++)
		{
			if (StaticItemSpawnPoints[i])
			{
				StaticItemSpawnPoints[i]->OnItemPurchased.AddDynamic(this, &ANPCShop::IncreaseStoreFunds);
			}
		}
	}
}

void ANPCShop::RotateStock()
{
	if (RotatingItemSpawnPoints.Num() > 0)
	{
		for (int i = 0; i < RotatingItemSpawnPoints.Num(); i++)
		{
			SpawnRandomItem(RotatingItemSpawnPoints[i]);
		}
		SaveCurrentStock();
	}
}


void ANPCShop::RestockItems()
{
	if (!HasAuthority()) return;
	
	if (StaticItemSpawnPoints.Num() > 0)
	{
		for (int i = 0; i < StaticItemSpawnPoints.Num(); i++)
		{
			if (StaticItemSpawnPoints[i] && StaticItemSpawnPoints[i]->GetCurrentItem())
			{
				StaticItemSpawnPoints[i]->SpawnItem();
			}
		}
	}
	RotateStock();

	
}

void ANPCShop::SaveCurrentStock()
{
	
}

void ANPCShop::IncreaseStoreFunds(float MoneySpent)
{
	TotalMoneySpentAtStore += MoneySpent;
}

void ANPCShop::DonateMoneyToStore(float MoneyDonated)
{
	TotalMoneyDonated += MoneyDonated;
}


// Called every frame
void ANPCShop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

