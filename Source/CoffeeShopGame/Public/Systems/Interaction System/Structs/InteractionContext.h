#pragma once

#include "Characters/Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Systems/PickupDropSystem/HolderComponent/HolderComponent.h"
#include "Systems/StatModificationSystem/Components/StatHandlerComponent.h"
#include "InteractionContext.generated.h"

class UMovementComponent;
class UInteractionComponent;
class UBuildingComponent;

USTRUCT(BlueprintType)
struct FInteractionContext
{
	GENERATED_BODY()


	//Instigator Basics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	APlayerCharacter* Instigator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	AController* InstigatorController = nullptr;


	//Instigator Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UStatHandlerComponent* StatHandlerComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UCharacterMovementComponent* MovementComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UInteractionComponent* InteractionComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UHolderComponent* HolderComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UBuildingComponent* BuildingComponent = nullptr;


	//Actor Interacted With
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General")
	AActor* FocusActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General")
	FVector LookedAtLocation;
	

	//Constructor
	FInteractionContext() = default;
	FInteractionContext(APlayerCharacter* _Instigator, AController* _InstigatorController,
		UCharacterMovementComponent* _MovementComponent, UInteractionComponent* _InteractionComponent, UHolderComponent* _HolderComponent,
		AActor* _FocusActor, FVector _LookedAtLocation)
	{
		Instigator = _Instigator;
		InstigatorController = _InstigatorController;
		MovementComponent = _MovementComponent;
		InteractionComponent = _InteractionComponent;
		HolderComponent = _HolderComponent;
		FocusActor = _FocusActor;
		LookedAtLocation = _LookedAtLocation;
	}
};