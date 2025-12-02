// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/Building System/Components/BuildingComponent.h"
#include "Core/Framework/Subsystems/LookTraceSubsystem.h"
#include "Systems/Building System/Actors/Furniture.h"
#include "Systems/Building System/Subsystems/BuildingSubsystem.h"
#include "Systems/Building System/Actors/GridVisualizer.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UBuildingComponent::UBuildingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UBuildingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Player = Cast<APawn>(GetOwner());
	if (Player) Controller = Player->GetController();

	TraceSubsystem = GetWorld()->GetSubsystem<ULookTraceSubsystem>();

	BuildingSubsystem = GetWorld()->GetSubsystem<UBuildingSubsystem>();

	if (!Player->IsLocallyControlled())
		return;

	PreviewActor = GetWorld()->SpawnActor<APreviewActor>(PreviewActorClass);

	GridVisualizer = Cast<AGridVisualizer>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridVisualizer::StaticClass()));
}


// Called every frame
void UBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Furniture || !Player->IsLocallyControlled())
		return;

	FVector TraceHitLocation = TraceSubsystem->GetLocationFromLineTrace(Controller);

	if (!BuildingSubsystem)
	{
		PlaceLocation = TraceHitLocation;
		bOnGrid = false;
	}

	if (BuildingSubsystem->GetClosestLocation(TraceHitLocation, PlaceLocation))
	{
		PlaceLocation.Z = TraceHitLocation.Z;
		bOnGrid = true;
	}
	else
	{
		PlaceLocation = TraceHitLocation;
		bOnGrid = false;
	}

	PreviewActor->SetActorLocation(PlaceLocation + Furniture->GetData()->OffsetFromOrigin);
	bool CanPlace = BuildingSubsystem->CanPlaceFurniture(PlaceLocation, Furniture->GetData());
	PreviewActor->SetCanPlacePreview(CanPlace);
	GridVisualizer->UpdatePlacableStatus(CanPlace);

	if (bOnGrid)
		GridVisualizer->UpdatePreviewMeshLocation(PlaceLocation);
}

void UBuildingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UBuildingComponent, Furniture, COND_OwnerOnly);
}

void UBuildingComponent::SetBuildingPreviewActor(AFurniture* InPreviewActor)
{
	BuildingSubsystem->SetTilesWithDataAsset(InPreviewActor->GetActorLocation() - InPreviewActor->GetData()->OffsetFromOrigin, InPreviewActor->GetData(), true);
	Furniture = InPreviewActor;

	if (GetOwner()->HasAuthority() && Player->IsLocallyControlled())
		SetPreviewMesh();
}

void UBuildingComponent::SetPreviewMesh()
{
	if (!Player->IsLocallyControlled())
		return;

	if (!Furniture)
		PreviewActor->SetActorHiddenInGame(true);
	else
	{
		PreviewActor->SetActorHiddenInGame(false);
		PreviewActor->SetMesh(Furniture->GetMeshComponent()->GetStaticMesh());
		PreviewActor->SetActorScale3D(Furniture->GetData()->Scale);
		GridVisualizer->UpdatePreviewMesh(Furniture->GetData());
		GridVisualizer->EnablePreview(true);
	}
}

bool UBuildingComponent::ResetBuildingPreview(FVector LookAtLocation, FVector& OutPlaceLocation)
{
	if (PlaceLocation == FVector::ZeroVector || !bOnGrid)
		return false;

	if (!BuildingSubsystem->CanPlaceFurniture(PlaceLocation, Furniture->GetData()))
		return false;

	BuildingSubsystem->SetTilesWithDataAsset(PlaceLocation, Furniture->GetData(), false);
	FVector Offset = Furniture->GetData()->OffsetFromOrigin;
	Furniture = nullptr;
	if (GetOwner()->HasAuthority() && Player->IsLocallyControlled())
		SetPreviewMesh();

	GridVisualizer->EnablePreview(false);

	OutPlaceLocation = PlaceLocation + Offset;
	return true;
}

AFurniture* UBuildingComponent::GetBuildingPreviewActor()
{
	return Furniture;
}

void UBuildingComponent::SetOnGrid_Implementation(bool IsOnGrid)
{
	//???
}

