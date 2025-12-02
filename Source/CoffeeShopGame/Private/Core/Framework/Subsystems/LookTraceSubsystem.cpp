#include "Core/Framework/Subsystems/LookTraceSubsystem.h"

void ULookTraceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LookTraceSettings = GetMutableDefault<ULookTraceSettings>();
}

FHitResult ULookTraceSubsystem::GetHitResultFromControllerCameraSphereTrace(AController* Controller)
{
	FHitResult Hit;

	if (!LookTraceSettings) return Hit;

	float TraceRadius = LookTraceSettings->GetTraceRadius();

	//Start and End Locations
	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	FVector StartLocation = ViewLocation;
	FVector EndLocation = ViewLocation + ViewRotation.Vector() * LookTraceSettings->GetTraceLength();
	
	//Sphere, Params
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SphereTraceSingle),false);

	//Shoot sphere trace
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, StartLocation, EndLocation,
		FQuat::Identity, LookTraceSettings->GetTraceChannel(), Sphere, Params
	);

	//Debug
	if (LookTraceSettings->DoesDrawDebug())
	{
		DrawDebugCapsule(
			GetWorld(), (StartLocation+EndLocation)*0.5f,
			(EndLocation-StartLocation).Size()*0.5f,TraceRadius,
			FRotationMatrix::MakeFromZ(ViewRotation.Vector()).ToQuat(),
			bHit ? FColor::Green : FColor::Red, false, 0.05f
		);
	}
	
	return Hit; //kinda wanna return bHit as well hmmmm
}

FHitResult ULookTraceSubsystem::GetHitResultFromTargetSphereTrace(AActor* Tracer, FVector TraceDirection, float TraceLength,
	float TraceRadius, TEnumAsByte<ECollisionChannel> TraceChannel)
{
	FHitResult Hit;

	if (!LookTraceSettings) return Hit;

	//Start and End Locations
	FVector TraceStartLoc = FVector::ZeroVector;
	if (Tracer) TraceStartLoc = Tracer->GetActorLocation();
	FVector StartLocation = TraceStartLoc;
	FVector EndLocation = TraceStartLoc + TraceDirection * TraceLength;
	
	//Sphere, Params
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SphereTraceSingle),false, Tracer);

	//Shoot sphere trace
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, StartLocation, EndLocation,
		FQuat::Identity, TraceChannel, Sphere, Params
	);

	//Debug
	if (LookTraceSettings->DoesDrawDebug())
	{
		DrawDebugCapsule(
			GetWorld(), (StartLocation+EndLocation)*0.5f,
			(EndLocation-StartLocation).Size()*0.5f,TraceRadius,
			FRotationMatrix::MakeFromZ(TraceDirection).ToQuat(),
			bHit ? FColor::Green : FColor::Red, false, 0.05f
		);
	}
	
	return Hit;
}

FVector ULookTraceSubsystem::GetLocationFromLineTrace(AController* Controller) //gotta make this a line, not a sphere
{
	FHitResult Hit;

	if (!LookTraceSettings) return FVector::ZeroVector;

	float TraceRadius = LookTraceSettings->GetTraceRadius();

	//Start and End Locations
	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	FVector StartLocation = ViewLocation;
	FVector EndLocation = ViewLocation + ViewRotation.Vector() * LookTraceSettings->GetTraceLength();
	
	//Sphere, Params
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SphereTraceSingle),false);

	//Shoot sphere trace
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, StartLocation, EndLocation,
		FQuat::Identity, LookTraceSettings->GetTraceChannel(), Sphere, Params
	);

	//Debug
	if (LookTraceSettings->DoesDrawDebug())
	{
		DrawDebugCapsule(
			GetWorld(), (StartLocation+EndLocation)*0.5f,
			(EndLocation-StartLocation).Size()*0.5f,TraceRadius,
			FRotationMatrix::MakeFromZ(ViewRotation.Vector()).ToQuat(),
			bHit ? FColor::Green : FColor::Red, false, 0.05f
		);
	}

	return Hit.ImpactPoint; //kinda wanna return bHit as well hmmmm
}
