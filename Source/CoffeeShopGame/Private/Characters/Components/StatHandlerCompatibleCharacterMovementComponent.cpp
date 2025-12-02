#include "Characters/Components/StatHandlerCompatibleCharacterMovementComponent.h"
#include "Systems/StatModificationSystem/GameTags.h"


UStatHandlerCompatibleCharacterMovementComponent::UStatHandlerCompatibleCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatHandlerCompatibleCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	
	//get components
	StatHandler = GetOwner()->GetComponentByClass<UStatHandlerComponent>();


	//set initial stats
	// move speed
	float ModifiedMoveSpeed = 0.0f;
	bool bFoundStat = StatHandler->TryGetModifiedPropertyStat(TAG_PropertyStat_Player_MoveSpeed, ModifiedMoveSpeed);
	if (bFoundStat) MaxWalkSpeed = ModifiedMoveSpeed;


	//subscribe to delegates
	StatHandler->OnStatModified.AddDynamic(this, &UStatHandlerCompatibleCharacterMovementComponent::OnStatModified);
}

void UStatHandlerCompatibleCharacterMovementComponent::OnStatModified()
{
	//move speed
	float ModifiedMoveSpeed = 0.0f;
	bool bFoundStat = StatHandler->TryGetModifiedPropertyStat(TAG_PropertyStat_Player_MoveSpeed, ModifiedMoveSpeed);
	if (bFoundStat) MaxWalkSpeed = ModifiedMoveSpeed;
}
