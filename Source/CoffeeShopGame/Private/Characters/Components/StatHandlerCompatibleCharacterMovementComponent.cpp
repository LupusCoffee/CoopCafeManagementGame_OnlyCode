#include "Characters/Components/StatHandlerCompatibleCharacterMovementComponent.h"

#include "Net/UnrealNetwork.h"
#include "Systems/StatModificationSystem/GameTags.h"


//Setup
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


//Subscribed Functions
void UStatHandlerCompatibleCharacterMovementComponent::OnStatModified()
{
	//move speed
	float ModifiedMoveSpeed = 0.0f;
	bool bFoundStat = StatHandler->TryGetModifiedPropertyStat(TAG_PropertyStat_Player_MoveSpeed, ModifiedMoveSpeed);
	if (bFoundStat) MaxWalkSpeed = ModifiedMoveSpeed;
}


//Public Functions
void UStatHandlerCompatibleCharacterMovementComponent::BlockMovementInput(bool bInput)
{
	bBlockMovementInput = bInput;
}

void UStatHandlerCompatibleCharacterMovementComponent::BlockJumpInput(bool bInput)
{
	bBlockJumpInput = bInput;
}

void UStatHandlerCompatibleCharacterMovementComponent::BlockMovementAndJumpInput(bool bInput)
{
	bBlockMovementInput = bInput;
	bBlockJumpInput = bInput;
}

bool UStatHandlerCompatibleCharacterMovementComponent::IsBlockingMovementInput()
{
	return bBlockMovementInput;
}

bool UStatHandlerCompatibleCharacterMovementComponent::IsBlockingJumpInput()
{
	return bBlockJumpInput;
}


//Replication
void UStatHandlerCompatibleCharacterMovementComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatHandlerCompatibleCharacterMovementComponent, bBlockMovementInput);
	DOREPLIFETIME(UStatHandlerCompatibleCharacterMovementComponent, bBlockJumpInput);
}

bool UStatHandlerCompatibleCharacterMovementComponent::IsSupportedForNetworking() const
{
	return true;
}
