// Fill out your copyright notice in the Description page of Project Settings.


#include "TableManager.h"

#include "UCustomerAISubsystem.h"

void ATableManager::AddTable(ACoffeeShopTable* Table)
{
	if (!this)
	{
		UE_LOG(LogTemp, Error, TEXT("TableManager pointer is invalid!"));
		return;
	}

	if (!Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("Table pointer is null!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Adding table: %s"), *Table->GetName());

	tables.Add(Table);
}

void ATableManager::RemoveTable(ACoffeeShopTable* Table)
{
	tables.Remove(Table);
}

ACoffeeShopTable* ATableManager::GetRandomTable()
{
	int n = FMath::RandRange(0, tables.Num()-1);    
	return tables[n];
}

ACoffeeShopTable* ATableManager::GetTable(int n)
{
	if (n >= tables.Num())
		return nullptr;
	
	return tables[n];
}

void ATableManager::BeginPlay()
{
	Super::BeginPlay();
}
