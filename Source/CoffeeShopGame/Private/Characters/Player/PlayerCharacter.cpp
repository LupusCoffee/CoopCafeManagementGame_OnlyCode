#include "Characters/Player/PlayerCharacter.h"

#include "Characters/Components/StatHandlerCompatibleCharacterMovementComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/InteractionComponent.h"


APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStatHandlerCompatibleCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	HolderComponent = CreateDefaultSubobject<UHolderComponent>(TEXT("HolderComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	StatHandlableMovementComponent = Cast<UStatHandlerCompatibleCharacterMovementComponent>(GetCharacterMovement());
}

UInteractionComponent* APlayerCharacter::GetInteractionComponent()
{
	return InteractionComponent;
}

UHolderComponent* APlayerCharacter::GetHolderComponent()
{
	return HolderComponent;
}

UStatHandlerCompatibleCharacterMovementComponent* APlayerCharacter::GetStatHandleableCharMoveComp()
{
	return StatHandlableMovementComponent;
}

