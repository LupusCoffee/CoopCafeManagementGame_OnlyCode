// Fill out your copyright notice in the Description page of Project Settings.

#include "Customer.h"

#include "CoffeeShopGame/Managers/LoopManagementSubsystem.h"
#include "CoffeeShopGame/Managers/TableManagerSubsystem.h"
#include "CoffeeShopGame/Customer/Misq/Chair.h"
#include "GOAPSystem/AI/Interface/Interractable.h"
#include "Misq/TimerComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(CoffeeNet, Log, All);

ACustomer::ACustomer()
{
	bReplicates = true;
}

void ACustomer::BeginPlay()
{
	Super::BeginPlay();

	LoopManagementSubsystem = GetWorld()->GetSubsystem<ULoopManagementSubsystem>();
	TableManagerSubsystem = GetWorld()->GetSubsystem<UTableManagerSubsystem>();

}

void ACustomer::SetTargetActor(AActor* _targetActor)
{
	if (!HasAuthority())
	{
		if (IsValid(_targetActor))
		{
			Server_SetTargetActor(_targetActor);
		}
		return;
	}

	if (IsValid(_targetActor))
	{
		TargetActor = _targetActor;
		OnNewTargetAssigned.Broadcast(TargetActor);
	}
	else
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to set target actor but it's not valid!"));
}

AActor* ACustomer::GetTargetActor()
{
	return TargetActor;
}

void ACustomer::MoveToTargetActor()
{
	if (!HasAuthority())
	{
		Server_MoveToTargetActor();
		return;
	}

	if (!GetTargetActor())
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to move to target actor but it's not valid!"));
		return;
	}

	if (!QueueController.IsValid())
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to move to target actor but QueueController is invalid!"));
		return;
	}

	QueueController->MoveToActor(TargetActor, 15.0f);
	OnMoveStarted.Broadcast();
}

void ACustomer::MoveToTargetPosition(FVector TargetPosition)
{
	if (!HasAuthority())
	{
		Server_MoveToTargetActor();
		return;
	}

	if (!QueueController.IsValid())
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to move to target Position but QueueController is invalid!"));
		return;
	}

	QueueController->MoveToPosition(TargetPosition, 15.0f);
	OnMoveStarted.Broadcast();
}

bool ACustomer::InteractWithTargetActor(bool bUseGOAPInteract)
{
	if (!HasAuthority())
	{
		return false;
	}

	if (!GetTargetActor())
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to interact with target actor but it's not valid!"));
		return false;
	}
	
	if (bUseGOAPInteract)
	{
		if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UGOAPInteractable::StaticClass()))
		{
			IGOAPInteractable::Execute_Interact(TargetActor, this);
			UE_LOG(CoffeeNet, Warning, TEXT("Customer: GOAP interacted with target actor %s"), *TargetActor->GetName());
			return true;
		}

		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried GOAP interaction but target actor is not IGOAPInteractable! %s"), *GetNameSafe(TargetActor));
		return false;
	}

	if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		IInteractable::Execute_AIInteract(TargetActor, this);
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Interacted with target actor %s"), *TargetActor->GetName());
		return true;
	}
	
	UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to interact with target actor but it's not a IInteractable!"));
	return false;
}

void ACustomer::MoveToTargetActorAndInteract(bool bUseGOAPInteract)
{
	if (!HasAuthority())
	{
		Server_MoveToTargetActorAndInteract(bUseGOAPInteract);
		return;
	}

	if (!GetTargetActor())
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to move to target actor but it's not valid!"));
		return;
	}

	if (!QueueController.IsValid())
	{
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Tried to move to target actor but QueueController is invalid!"));
		return;
	}
	
	if(bIsMovingToTargetActor)
		return;
	
	FVector MoveLocation = TargetActor->GetActorLocation();
	if (AChair* Chair = Cast<AChair>(TargetActor))
	{
		MoveLocation = Chair->GetChairMovePoint()->GetComponentLocation();
	}

	bUseGOAPInteractForPendingMove = bUseGOAPInteract;
	QueueController->MoveToPosition(MoveLocation, MovementAcceptanceRadius);
	QueueController->MoveCompleted.AddDynamic(this, &ACustomer::PrivateInteractWIthTargetActor);
	
	bIsMovingToTargetActor = true;		
	OnMoveStarted.Broadcast();
}

void ACustomer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACustomer::StaticClass());

	bool bBumped = false;
	for (AActor* Other : OverlappingActors)
	{
		if (Other == this) continue;

		FVector PushDirection = GetActorLocation() - Other->GetActorLocation();
		PushDirection.Z = 0.f;
		float Distance = PushDirection.Size();
		PushDirection.Normalize();
		float PushStrength = FMath::Clamp(1.f - (Distance / PushRadius), 0.f, 1.f);
		GetCharacterMovement()->AddInputVector(PushDirection * PushStrength * PushForce * DeltaSeconds);

		// If we are close enough, consider it a bump
		if (PushStrength > 0.0f)
		{
			bBumped = true;
		}
	}

	// Handle bump event with cooldown
	if (bBumped)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastBumpTime >= BumpCooldown)
		{
			OnAIBumped.Broadcast();
			LastBumpTime = CurrentTime;
		}
	}
}

void ACustomer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACustomer, DesiredDrink);
	DOREPLIFETIME(ACustomer, bIsInQueue);
	DOREPLIFETIME(ACustomer, bIsSitting);
	DOREPLIFETIME(ACustomer, TargetActor);
}

bool ACustomer::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{	
	if (!HasAuthority())
	{
		Server_ProcessInteraction(false);
		return true;
	}

	ProcessInteractionOnServer(false);
	
	return IInteractable::Server_StartInteraction_Implementation(ActionId, Context);
}

void ACustomer::Server_MoveToTargetPosition_Implementation(FVector targetPosition)
{
	MoveToTargetPosition(targetPosition);
}

void ACustomer::ProcessInteractionOnServer(bool GOAPInteract)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!bIsInQueue)
	{
		OnInteractionReceived.Broadcast(this);
		UE_LOG(CoffeeNet, Warning, TEXT("Customer: Interacted with something but is not in queue, just broadcasting interaction received!"));
		return;
	}

	UE_LOG(CoffeeNet, Warning, TEXT("Customer: Interacted with something while in queue, leaving queue and moving to target actor!"));
	if (UTimerComponent* Timer = GetComponentByClass<UTimerComponent>())
	{
		Timer->PauseTimer();
	}

	if (QueueController.IsValid() && QueueController->GetQueue())
	{
		QueueController->GetQueue()->CustomerLeavesQueue(this);
	}
	

	GetFreeChair();
	MoveToTargetActorAndInteract(GOAPInteract);
}

void ACustomer::HandleLoopFinished()
{
	if (!GoapComp)
	{
		return;
	}

	if (!HasAuthority())
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("[ServerOnly][NetAuth][Customer] HandleLoopFinished ignored on client for %s"), *GetName());
		return;
	}

	GoapComp->NotifyInteractionReceived();
}

void ACustomer::GenerateDrinkOrder()
{
	if (!HasAuthority())
	{
		return;
	}
		
	switch (FMath::RandRange(0, 4))
	{
		case 0:
		DesiredDrink = EResourceType::Milk;
		break;
		case 1:
		DesiredDrink = EResourceType::BasicCoffee;
		break;
		case 2:
		DesiredDrink = EResourceType::BasicCoffeeWithMilk;
		break;
		case 3:
		DesiredDrink = EResourceType::SpiderCoffee;
		break;
		case 4:
		DesiredDrink = EResourceType::SpiderCoffeeWithMilk;
		break;
	default: 
		DesiredDrink = EResourceType::BasicCoffee;
	}
	
	OnDrinkGenerated.Broadcast(DesiredDrink);
}

EResourceType ACustomer::GetDrinkOrder() const
{
	return DesiredDrink;
}

float ACustomer::GenerateTipAmount_Implementation()
{
	return 0;
}

void ACustomer::PrivateInteractWIthTargetActor(ACustomer* FinishedCustomer)
{
	if (!HasAuthority())
	{
		return;
	}

	if (FinishedCustomer && FinishedCustomer != this)
	{
		return;
	}

	InteractWithTargetActor(bUseGOAPInteractForPendingMove);
	
	bUseGOAPInteractForPendingMove = false;
	bIsMovingToTargetActor = false;
	if (QueueController.IsValid())
	{
		QueueController->MoveCompleted.RemoveDynamic(this, &ACustomer::PrivateInteractWIthTargetActor);
	}
}

void ACustomer::GetFreeChair()
{
	if (!HasAuthority())
	{
		UE_LOG(CoffeeNet, Verbose, TEXT("[ServerOnly][NetAuth][Customer] GetFreeChair ignored on client for %s"), *GetName());
		return;
	}

	if (auto* tableSubSystem = GetWorld()->GetSubsystem<UTableManagerSubsystem>())
	{
		AChair* chair = tableSubSystem->RequestAvailableChair(this, true);
		
		if (!chair)
			return;
		
		SetTargetActor(chair);
		
		OnOrderTaken.Broadcast(GetDrinkOrder(), chair);
	}
}

void ACustomer::Server_ProcessInteraction_Implementation(bool GOAPInteract)
{
	ProcessInteractionOnServer(GOAPInteract);
}

void ACustomer::Server_SetTargetActor_Implementation(AActor* InTargetActor)
{
	SetTargetActor(InTargetActor);
}

void ACustomer::Server_MoveToTargetActor_Implementation()
{
	MoveToTargetActor();
}

void ACustomer::Server_MoveToTargetActorAndInteract_Implementation(bool GOAPInteract)
{
	MoveToTargetActorAndInteract(GOAPInteract);
}

