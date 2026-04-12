#pragma once

#include "Characters/Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HolderComponent.h"
#include "Systems/StatModificationSystem/Components/StatHandlerComponent.h"
#include "PlayerContext.generated.h"

class UPlayerPromptComponent;
class UMovementComponent;
class UInteractionComponent;
class UBuildingComponent;

USTRUCT(BlueprintType)
struct FPlayerContext
{
	GENERATED_BODY()


	//Interactor Basics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	APlayerCharacter* Interactor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	AController* InteractorController = nullptr;


	//Interactor Components
	// basics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UInteractionComponent* InteractionComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UHolderComponent* HolderComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UCharacterMovementComponent* MovementComponent = nullptr;
	
	// ???
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UStatHandlerComponent* StatHandlerComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UBuildingComponent* BuildingComponent = nullptr;
	
	// visual
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UPlayerPromptComponent* PlayerPromptComponent = nullptr;


	//Looked at
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General")
	AActor* LookedAtActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General")
	FVector LookedAtLocation;
	
	//Current Highlight Component
	

	//Constructor
	FPlayerContext() = default;
	FPlayerContext(APlayerCharacter* _Instigator, AController* _InstigatorController,
		UCharacterMovementComponent* _MovementComponent, UInteractionComponent* _InteractionComponent, UHolderComponent* _HolderComponent,
		AActor* InLookedAtActor, FVector InLookedAtLocation)
	{
		Interactor = _Instigator;
		InteractorController = _InstigatorController;
		
		InteractionComponent = _InteractionComponent;
		HolderComponent = _HolderComponent;
		MovementComponent = _MovementComponent;
		
		LookedAtActor = InLookedAtActor;
		LookedAtLocation = InLookedAtLocation;
	}
};