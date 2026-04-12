// Fill out your copyright notice in the Description page of Project Settings.


#include "ActivatableComponent.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GOAPSystem/GOAP/Components/GoapComponent.h"
#include "GOAPSystem/AI/Utils/AIComponentUtils.h"


// Sets default values for this component's properties
UActivatableComponent::UActivatableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Enable replication
 	SetIsReplicated(true);

	// ...
}

void UActivatableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate the active state to all clients
	DOREPLIFETIME(UActivatableComponent, bIsActiveActor);
}

void UActivatableComponent::BeginPlay()
{	
	Super::BeginPlay();
	
	if (UCharacterMovementComponent* Movement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>())
	{
		baseGravityScale = Movement->GravityScale;
	}

	if (GetOwnerRole() != ROLE_Authority)
	{
		// Clients should mirror server state via replication, not invoke server RPC init paths.
		OnRep_IsActiveActor();
		return;
	}
	
	if (!bIsActiveActor)
	{
		bIsActiveActor = true;
		DeactivateActor();
	}
	else
	{
		bIsActiveActor = false;
		ActivateActor();
	}
	
	
}

void UActivatableComponent::OnRep_IsActiveActor()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UCharacterMovementComponent* Movement = Owner->FindComponentByClass<UCharacterMovementComponent>();
	FAIComponentUtils::FAIComponents AIComponents = FAIComponentUtils::GetAllAIComponents(Owner);

	if (bIsActiveActor)
	{
		if (bHideWhenInactive)
		{
			Owner->SetActorHiddenInGame(false);
		}

		if (bDisableCollisionWhenInactive)
		{
			Owner->SetActorEnableCollision(true);
		}

		if (bDisableTickWhenInactive)
		{
			Owner->SetActorTickEnabled(true);
			PrimaryComponentTick.SetTickFunctionEnable(true);
			if (Movement)
			{
				Movement->PrimaryComponentTick.SetTickFunctionEnable(true);
			}
		}

		if (AIComponents.GoapComp)
		{
			AIComponents.GoapComp->SetComponentTickEnabled(true);
		}

		if (Movement)
		{
			Movement->SetMovementMode(MOVE_Walking);
			if (bShouldBeAffectedByGravity)
			{
				Movement->GravityScale = baseGravityScale;
			}
		}

		OnActivated.Broadcast();
		return;
	}

	if (bHideWhenInactive)
	{
		Owner->SetActorHiddenInGame(true);
	}

	if (bDisableCollisionWhenInactive)
	{
		Owner->SetActorEnableCollision(false);
	}

	if (bDisableTickWhenInactive)
	{
		Owner->SetActorTickEnabled(false);
		PrimaryComponentTick.SetTickFunctionEnable(false);
		if (Movement)
		{
			Movement->PrimaryComponentTick.SetTickFunctionEnable(false);
		}
	}

	if (AIComponents.GoapComp)
	{
		AIComponents.GoapComp->SetComponentTickEnabled(false);
	}

	if (bShouldBeAffectedByGravity && Movement)
	{
		Movement->SetMovementMode(MOVE_None);
		Movement->GravityScale = 0.0f;
	}

	OnDeactivated.Broadcast();
}

void UActivatableComponent::ActivateActor()
{
	// Route to server if this is a client
	if (GetOwnerRole() != ROLE_Authority)
	{
		Server_ActivateActor();
		return;
	}

	if (IsActiveActor()) return;
	
	
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	// Get all AI components in one go
	FAIComponentUtils::FAIComponents AIComponents = FAIComponentUtils::GetAllAIComponents(Owner);
	UCharacterMovementComponent* Movement = Owner->FindComponentByClass<UCharacterMovementComponent>();
	
	//Restore visibility
	if (bHideWhenInactive)	
	{
		Owner->SetActorHiddenInGame(false);
	}
	
	//Restore collisions
	if (bDisableCollisionWhenInactive)
	{
		Owner->SetActorEnableCollision(true);
	}
	
	//Restore ticking
	if (bDisableTickWhenInactive)
	{
		Owner->SetActorTickEnabled(true);
		PrimaryComponentTick.SetTickFunctionEnable(true);
		
		if (Movement)
			Movement->PrimaryComponentTick.SetTickFunctionEnable(true);
	}

	if (AIComponents.GoapComp)
	{
		AIComponents.GoapComp->SetComponentTickEnabled(true);
		// On pooled reuse we want a clean runtime state, but avoid resetting before BeginPlay initializes defaults.
		if (AIComponents.GoapComp->HasBegunPlay())
		{
			AIComponents.GoapComp->ResetExecutionState();
			AIComponents.GoapComp->RefreshDefaultKnowledge();
		}
	}
	
	//Restore movement and gravity
	if (Movement)
	{
		// Always restore movement mode (it was set to MOVE_None on deactivation)
		Movement->SetMovementMode(MOVE_Walking);
		
		// Restore gravity if affected
		if (bShouldBeAffectedByGravity)
		{
			Movement->GravityScale = baseGravityScale;
		}
	}
	
	// Restore controller and AI Brain
	if (AIComponents.Controller)
	{
		if (bDisableTickWhenInactive)
		{
			AIComponents.Controller->SetActorTickEnabled(true);
		}

		if (AIComponents.Brain)
		{
			// Fully reset Behavior Tree: Stop -> Cleanup -> Start fresh
			AIComponents.Brain->StopLogic(TEXT("FullReset"));
			AIComponents.Brain->Cleanup();
			AIComponents.Brain->StartLogic();
			
			UE_LOG(LogTemp, Verbose, TEXT("ActivatableComponent: Brain fully reset for %s"), *Owner->GetName());
		}
	}
	
	bIsActiveActor = true;
	OnActivated.Broadcast();
}

void UActivatableComponent::DeactivateActor()
{
	// Route to server if this is a client
	if (GetOwnerRole() != ROLE_Authority)
	{
		Server_DeactivateActor();
		return;
	}

	if (!IsActiveActor()) return;
	
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	// Get all AI components in one go
	FAIComponentUtils::FAIComponents AIComponents = FAIComponentUtils::GetAllAIComponents(Owner);
	UCharacterMovementComponent* Movement = Owner->FindComponentByClass<UCharacterMovementComponent>();
	
	//Disable visibility
	if (bHideWhenInactive)	
	{
		Owner->SetActorHiddenInGame(true);
	}
	
	//Disable collisions
	if (bDisableCollisionWhenInactive)
	{
		Owner->SetActorEnableCollision(false);
	}
	
	//Disable ticking
	if (bDisableTickWhenInactive)
	{
		Owner->SetActorTickEnabled(false);
		PrimaryComponentTick.SetTickFunctionEnable(false);
		
		if (Movement)
			Movement->PrimaryComponentTick.SetTickFunctionEnable(false);
	}

	if (AIComponents.GoapComp)
	{
		AIComponents.GoapComp->SetComponentTickEnabled(false);
	}
	
	//Disable gravity
	if (bShouldBeAffectedByGravity && Movement)
	{
		Movement->SetMovementMode(MOVE_None);
		Movement->GravityScale = 0.0f;
	}
	
	// Handle AI Controller deactivation using utility functions
	if (AIComponents.Controller)
	{
		// Stop AI movement
		AIComponents.Controller->StopMovement();
		
		// Disable controller ticking if configured
		if (bDisableTickWhenInactive)
		{
			AIComponents.Controller->SetActorTickEnabled(false);
		}

		// Stop Brain logic
		if (AIComponents.Brain)
		{
			// Fully stop the Brain (not just pause) for complete reset on reactivation
			AIComponents.Brain->StopLogic(TEXT("Deactivated"));
		}
	}
	
	// Keep GOAP fully idle while inactive; activation already performs a fresh reset.
	if (AIComponents.GoapComp)
	{
		if (AIComponents.GoapComp->HasBegunPlay())
		{
			AIComponents.GoapComp->ResetExecutionState();
		}
	}
	
	bIsActiveActor = false;
	OnDeactivated.Broadcast();
}

void UActivatableComponent::ActivateAtLocation(FVector Location, FRotator Rotation)
{
	// Route to server if this is a client
	if (GetOwnerRole() != ROLE_Authority)
	{
		Server_ActivateAtLocation(Location, Rotation);
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	// Move to the new location first
	Owner->SetActorLocation(Location);
	Owner->SetActorRotation(Rotation);
	
	// Then activate
	ActivateActor();
}

void UActivatableComponent::DeactivateAndMoveToHiddenLocation(FVector InHiddenLocation)
{
	// Route to server if this is a client
	if (GetOwnerRole() != ROLE_Authority)
	{
		Server_DeactivateAndMoveToHiddenLocation(InHiddenLocation);
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	// Deactivate first
	DeactivateActor();
	
	// Then move to hidden location
	Owner->SetActorLocation(InHiddenLocation);
	Owner->SetActorRotation(FRotator::ZeroRotator);
	
	// Store the hidden location
	HiddenLocation = InHiddenLocation;
}

void UActivatableComponent::SetHiddenLocation(FVector Location)
{
	HiddenLocation = Location;
}

// Server-authoritative RPC implementations
bool UActivatableComponent::Server_ActivateActor_Validate()
{
	return true;
}

void UActivatableComponent::Server_ActivateActor_Implementation()
{
	ActivateActor();
	Multicast_OnActivationStateChanged();
}

bool UActivatableComponent::Server_DeactivateActor_Validate()
{
	return true;
}

void UActivatableComponent::Server_DeactivateActor_Implementation()
{
	DeactivateActor();
	Multicast_OnActivationStateChanged();
}

bool UActivatableComponent::Server_ActivateAtLocation_Validate(FVector Location, FRotator Rotation)
{
	return true;
}

void UActivatableComponent::Server_ActivateAtLocation_Implementation(FVector Location, FRotator Rotation)
{
	ActivateAtLocation(Location, Rotation);
	Multicast_OnActivationStateChanged();
}

bool UActivatableComponent::Server_DeactivateAndMoveToHiddenLocation_Validate(FVector InHiddenLocation)
{
	return true;
}

void UActivatableComponent::Server_DeactivateAndMoveToHiddenLocation_Implementation(FVector InHiddenLocation)
{
	DeactivateAndMoveToHiddenLocation(InHiddenLocation);
	Multicast_OnActivationStateChanged();
}

void UActivatableComponent::Multicast_OnActivationStateChanged_Implementation()
{
	// Called on all clients to ensure UI/visual state is synchronized
	// Add any client-side only logic here if needed
}

