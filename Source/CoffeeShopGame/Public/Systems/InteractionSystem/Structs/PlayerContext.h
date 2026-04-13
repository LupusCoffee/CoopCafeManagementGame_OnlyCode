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
	

	//Constructor
	FPlayerContext() = default;
	FPlayerContext(APlayerCharacter* InInstigator, AController* InInstigatorController,
		UCharacterMovementComponent* InMovementComponent, UInteractionComponent* InInteractionComponent, UHolderComponent* InHolderComponent,
		AActor* InLookedAtActor, FVector InLookedAtLocation)
	{
		Interactor = InInstigator;
		InteractorController = InInstigatorController;
		
		InteractionComponent = InInteractionComponent;
		HolderComponent = InHolderComponent;
		MovementComponent = InMovementComponent;
		
		LookedAtActor = InLookedAtActor;
		LookedAtLocation = InLookedAtLocation;
	}
};
