// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_OnlyNonFullChairs.h"

#include "CoffeeShopGame/CoffeeShopTables/CoffeeShopChair.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

UEnvQueryTest_OnlyNonFullChairs::UEnvQueryTest_OnlyNonFullChairs(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	TestPurpose = EEnvTestPurpose::Filter;
	FilterType = EEnvTestFilterType::Match;
	SetWorkOnFloatValues(true);

	FloatValueMin.DefaultValue = 0.9f;
	FloatValueMax.DefaultValue = 1.0f;
}

void UEnvQueryTest_OnlyNonFullChairs::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		ACoffeeShopChair* Chair = Cast<ACoffeeShopChair>(ItemActor);
		if (!Chair) continue;

		const bool bIsEmpty = Chair->IsEmpty();
		const float Score = bIsEmpty ? 1.0f : 0.0f;

		It.SetScore(TestPurpose, FilterType, Score, 0.9f, 1.0f);
	}
}
