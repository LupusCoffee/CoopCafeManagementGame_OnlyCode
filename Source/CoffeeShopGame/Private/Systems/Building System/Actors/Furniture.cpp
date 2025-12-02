// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/Building System/Actors/Furniture.h"
#include "Net/UnrealNetwork.h"
#include "Systems/Building System/Components/BuildingComponent.h"
#include "Systems/PickupDropSystem/HolderComponent/HolderComponent.h"

//General Stuff
bool AFurniture::PickUp(const FInteractionContext& Context)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	UBuildingComponent* BuildingComponent = Context.BuildingComponent;
	if (!BuildingComponent) return false;

	UHeldItem* HolderComponentHeldItem = HolderComponent->GetHeldItem();

	if (HolderComponentHeldItem) //pick up
		return false;

	BuildingComponent->SetBuildingPreviewActor(this);
	return Super::PickUp(Context);
}

bool AFurniture::Drop(const FInteractionContext& Context)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	UBuildingComponent* BuildingComponent = Context.BuildingComponent;
	if (!BuildingComponent) return false;

	UHeldItem* HolderComponentHeldItem = HolderComponent->GetHeldItem();
	FVector Location;

	if (BuildingComponent->ResetBuildingPreview(Context.LookedAtLocation, Location))
	{
		FInteractionContext NewContext(Context);
		NewContext.LookedAtLocation = Location;
		if (!Super::Drop(NewContext))
			return false;

		Multicast_ResetRotation();
		return true;
	}

	return false;
}

void AFurniture::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetStaticMesh(Data->Mesh);
	SetActorScale3D(Data->Scale);
}

UFurnitureData* AFurniture::GetData()
{
	return Data;
}


void AFurniture::Multicast_ResetRotation_Implementation()
{
	SetActorRotation(FRotator::ZeroRotator);
}
