// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/VehicleSystem/VehicleManager.h"
#include "Path.h"
#include "Systems/VehicleSystem/Actor/Vehicle.h"
#include "TimeSubsystem.h"

// Sets default values
AVehicleManager::AVehicleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVehicleManager::BeginPlay()
{
	Super::BeginPlay();

	if (!Events.IsEmpty())
		UpcomingEvent = 0;

	TArray<USceneComponent*> Components;
	PathMap->GetRootComponent()->GetChildrenComponents(true, Components);
	for (int i = 0; i < Components.Num(); i++)
	{
		UTrackSplineComponent* Spline = Cast<UTrackSplineComponent>(Components[i]);
		if (Spline)
			if (Spline->IsMainSpline() && Spline->IsSpawnPoint())
				AvailablePaths.Add(Spline);
	}
	
	UWorld* World = GetWorld();
	if (!World)
		return;

	for (int i = 0; i < PoolSize; i++)
	{
		AVehicle* NewVehicle = GetWorld()->SpawnActor<AVehicle>(VehicleClass);
		VehiclePool.Add(NewVehicle);
		NewVehicle->SetManager(this);
	}

	for (int i = 0; i < AvailablePaths.Num(); i++)
		RandIndex.Add(i);

	TimeSubsystem = World->GetSubsystem<UTimeSubsystem>();
	CurrentDay = TimeSubsystem->GetDate().Day;
}

// Called every frame
void AVehicleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTime CurrentTime = TimeSubsystem->GetTime();
	if (!FinishedEvents)
	{
		if (Events.IsValidIndex(UpcomingEvent))
		{
			if (CurrentTime >= Events[UpcomingEvent].Time)
			{
				Efficiency = Events[UpcomingEvent].Efficiency;
				UpcomingEvent++;

				if (UpcomingEvent >= Events.Num())
					FinishedEvents = true;
			}
		}
	}
	else
	{
		if (CurrentDay != TimeSubsystem->GetDate().Day)
		{
			FinishedEvents = false;
			UpcomingEvent = 0;
			CurrentDay = TimeSubsystem->GetDate().Day;
		}
	}

	if (AvailablePaths.IsEmpty())
		return;

	if (Timer >= SpawnDelay)
	{
		int Amount = FMath::Min(int(SpawnAmount * Efficiency), AvailablePaths.Num());
		int32 LastIndex = RandIndex.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				RandIndex.Swap(i, Index);
			}
		}

		for (int i = 0; i < Amount; i++)
		{
			if (VehiclePool.IsEmpty())
				for (int o = 0; o < 5; o++)
				{
					AVehicle* NewVehicle = GetWorld()->SpawnActor<AVehicle>(VehicleClass);
					VehiclePool.Add(NewVehicle);
					NewVehicle->SetManager(this);
				}

			AVehicle* SpawningVehicle = VehiclePool.Pop();

			UTrackSplineComponent* FollowingPath = AvailablePaths[RandIndex[i]];

			if (VehicleModels.Num() > 0)
			{
				int Index = FMath::RandRange(0, VehicleModels.Num() - 1);
				SpawningVehicle->GetMeshComp()->SetStaticMesh(VehicleModels[Index]);
			}

			SpawningVehicle->Activate(FollowingPath);

			ActiveVehicles.Add(SpawningVehicle);
		}

		Timer = 0;
	}
	else
	{
		Timer += DeltaTime;
	}
}

void AVehicleManager::DeactivateCar(AVehicle* Car)
{
	Car->Deactivate();
	VehiclePool.Add(Car);
	ActiveVehicles.Remove(Car);
}

