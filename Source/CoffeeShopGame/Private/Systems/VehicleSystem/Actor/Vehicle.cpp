// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/VehicleSystem/Actor/Vehicle.h"
#include "Systems/VehicleSystem/VehicleManager.h"

// Sets default values
AVehicle::AVehicle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AVehicle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Update(DeltaTime);
}

void AVehicle::Activate(UTrackSplineComponent* InFollowingTrack)
{
	if (!InFollowingTrack)
		return;

	FollowingTrack = InFollowingTrack;

	MeshComp->SetVisibility(true);
	ForwardOrder = FollowingTrack->GetForwardOrder();
	DistanceAtNextPoint = 0;

	if (ForwardOrder == EForwardOrder::Ascending)
	{
		NextPointIndex = 0;
		TravelledDistance = 0;
	}
	else
	{
		NextPointIndex = FollowingTrack->GetNumberOfSplinePoints() - 1;
		TravelledDistance = 0;
	}
}

void AVehicle::Deactivate()
{
	MeshComp->SetVisibility(false);
	TravelledDistance = 0;
	FollowingTrack = nullptr;
}

void AVehicle::Update(float DeltaTime)
{
	if (!FollowingTrack)
		return;

	TravelledDistance += Speed * DeltaTime;

	if(DistanceAtNextPoint <= TravelledDistance)
	{
		if (FollowingTrack->HaveConnectionAtPoint(NextPointIndex))
		{
			TArray<FTrackConnectionData>& Connections = FollowingTrack->GetConnectionDataAtPoint(NextPointIndex)->Connections;

			int RandMax = Connections.Num() - 1;
			RandMax += HasFinished() ? 0 : 1;
			int RandIndex = FMath::RandRange(0, RandMax);

			if (RandIndex < Connections.Num())
			{
				FTrackConnectionData& Data = Connections[RandIndex];

				if (!Data.OtherSpline->HaveReachedEnd(Data.EndPointIndex))
				{
					FollowingTrack = Data.OtherSpline;
					ForwardOrder = FollowingTrack->GetForwardOrder();
					NextPointIndex = Data.EndPointIndex;
					float Surplus = TravelledDistance - DistanceAtNextPoint;
					if (ForwardOrder == EForwardOrder::Ascending)
						TravelledDistance = FollowingTrack->GetDistanceAlongSplineAtSplinePoint(NextPointIndex) + Surplus;
					else
						TravelledDistance = FollowingTrack->GetSplineLength() - FollowingTrack->GetDistanceAlongSplineAtSplinePoint(NextPointIndex) + Surplus;
				}
			}
		}
		else if(HasFinished())
		{
			VehicleManager->DeactivateCar(this);  
			return;
		}

		NextPointIndex += (ForwardOrder == EForwardOrder::Ascending ? 1 : -1);

		if (ForwardOrder == EForwardOrder::Ascending)
			DistanceAtNextPoint = FollowingTrack->GetDistanceAlongSplineAtSplinePoint(NextPointIndex);
		else
			DistanceAtNextPoint = FollowingTrack->GetSplineLength() - FollowingTrack->GetDistanceAlongSplineAtSplinePoint(NextPointIndex);
	}

	float DistanceOnSpline = (ForwardOrder == EForwardOrder::Ascending ? TravelledDistance : FollowingTrack->GetSplineLength() - TravelledDistance);
	FRotator Rotation = FollowingTrack->GetRotationAtDistanceAlongSpline(DistanceOnSpline, ESplineCoordinateSpace::World);
	if (ForwardOrder == EForwardOrder::Descending)
		Rotation = FRotator(-Rotation.Pitch, Rotation.Yaw - 180, Rotation.Roll);

	SetActorLocationAndRotation(FollowingTrack->GetLocationAtDistanceAlongSpline(DistanceOnSpline, ESplineCoordinateSpace::World),
								Rotation);
}

bool AVehicle::HasFinished()
{
	if (FollowingTrack->GetForwardOrder() == EForwardOrder::Ascending)
		return NextPointIndex >= FollowingTrack->GetNumberOfSplinePoints() - 1;
	else
		return NextPointIndex <= 0;
}

void AVehicle::SetManager(AVehicleManager* InManager)
{
	VehicleManager = InManager;
}
