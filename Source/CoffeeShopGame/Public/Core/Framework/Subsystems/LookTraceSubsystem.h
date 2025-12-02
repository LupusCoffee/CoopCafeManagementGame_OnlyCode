#pragma once

#include "CoreMinimal.h"
#include "Core/Framework/DeveloperSettings/LookTraceSettings.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LookTraceSubsystem.generated.h"


UCLASS()
class COFFEESHOPGAME_API ULookTraceSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//Methods
	UFUNCTION()
	FHitResult GetHitResultFromControllerCameraSphereTrace(AController* Controller);

	UFUNCTION()
	FHitResult GetHitResultFromTargetSphereTrace(AActor* Tracer, FVector TraceDirection, float TraceLength,
		float TraceRadius, TEnumAsByte<ECollisionChannel> TraceChannel);

	UFUNCTION()
	FVector GetLocationFromLineTrace(AController* Controller);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//Variables
	UPROPERTY()
	ULookTraceSettings* LookTraceSettings;
};
