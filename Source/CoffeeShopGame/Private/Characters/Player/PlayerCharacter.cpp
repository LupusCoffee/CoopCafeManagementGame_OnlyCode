#include "Characters/Player/PlayerCharacter.h"

#include "Characters/Components/StatHandlerCompatibleCharacterMovementComponent.h"
#include "Systems/Interaction System/Components/InteractionComponent.h"


APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStatHandlerCompatibleCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	HolderComponent = CreateDefaultSubobject<UHolderComponent>(TEXT("HolderComponent"));
}

UInteractionComponent* APlayerCharacter::GetInteractionComponent()
{
	return InteractionComponent;
}

UHolderComponent* APlayerCharacter::GetHolderComponent()
{
	return HolderComponent;
}

