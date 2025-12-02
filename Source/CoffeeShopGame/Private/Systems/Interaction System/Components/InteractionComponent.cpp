#include "Systems/Interaction System/Components/InteractionComponent.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Net/UnrealNetwork.h"
#include "Systems/Building System/Components/BuildingComponent.h"
#include "Systems/PickupDropSystem/HolderComponent/HolderComponent.h"


class UHighlightRegistrySubsystem;

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	//todo: refactor a bit --> make em into functions
	
	APawn* Pawn = Cast<APawn>(GetOwner());

	if (Pawn) Controller = Pawn->GetController();
	
	if (!Controller) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red,
		FString::Printf(TEXT("No controller for actor '%s'"), *GetOwner()->GetName()));
	
	if (Controller) IsPlayer = Controller->IsPlayerController();

	LookTraceSubsystem = GetWorld()->GetSubsystem<ULookTraceSubsystem>();
	
	if (IsPlayer) SetupUpdate();
	
	SetupInteractionContext();

	HighlightRegistry = GetWorld()->GetSubsystem<UHighlightRegistrySubsystem>();
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(FocusTimer);
	Super::EndPlay(EndPlayReason);
}

void UInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentDeltaTime = DeltaTime;

	UpdateFocusActorAndLookedAtLocation();
}


//Setup
void UInteractionComponent::SetupUpdate()
{
	if (UpdateFocusActorFrequency <= 0.0f)
	{
		PrimaryComponentTick.bCanEverTick = true;
		SetComponentTickEnabled(true);
	}
	else
	{
		const float Interval = 1.f / UpdateFocusActorFrequency;
		GetWorld()->GetTimerManager().SetTimer(FocusTimer, this, &UInteractionComponent::UpdateFocusActorAndLookedAtLocation, Interval, true);
	}	
}

void UInteractionComponent::SetupInteractionContext()
{
	InteractionContext.Instigator = Cast<APlayerCharacter>(GetOwner());	//should i account for the player character being an ai?
	InteractionContext.InstigatorController = Controller;

	InteractionContext.StatHandlerComponent = GetOwner()->GetComponentByClass<UStatHandlerComponent>();
	InteractionContext.MovementComponent = GetOwner()->GetComponentByClass<UCharacterMovementComponent>();
	InteractionContext.InteractionComponent = GetOwner()->GetComponentByClass<UInteractionComponent>();
	InteractionContext.HolderComponent = GetOwner()->GetComponentByClass<UHolderComponent>();
	InteractionContext.BuildingComponent = GetOwner()->GetComponentByClass<UBuildingComponent>();
}


//Update
//todo: refactor some of these into functions
void UInteractionComponent::UpdateFocusActorAndLookedAtLocation()
{
	if (!Controller || !LookTraceSubsystem) return;

	FHitResult Hit = LookTraceSubsystem->GetHitResultFromControllerCameraSphereTrace(Controller);
	LookedAtLocation = Hit.Location; //if problem, check here
	
	AActor* HitActor = Hit.GetActor();
	if (HitActor && !HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		HitActor = nullptr;
	}
	
	if (IsPlayer) UpdateHighlight(HitActor);
	
	FocusActor = HitActor;
}

void UInteractionComponent::UpdateHighlight(AActor* HitActor)
{
	if (!FocusActor && HitActor)
	{
		IInteractable::Execute_Hover(HitActor, InteractionContext);
		
		FLinearColor Color = FLinearColor::Green;
		if(HighlightRegistry) Color = HighlightRegistry->GetCurrentHighlightColor();
		OnHighlightEnabled.Broadcast(Color);
	}
	
	else if (FocusActor && !HitActor)
	{
		IInteractable::Execute_Unhover(FocusActor, InteractionContext);
		
		OnHighlightDisabled.Broadcast();
	}
	
	else if (FocusActor && HitActor && FocusActor != HitActor)
	{
		IInteractable::Execute_Unhover(FocusActor, InteractionContext);
		IInteractable::Execute_Hover(HitActor, InteractionContext);
		
		FLinearColor Color = FLinearColor::White;
		if(HighlightRegistry) Color = HighlightRegistry->GetCurrentHighlightColor();
		OnHighlightEnabled.Broadcast(Color);
	}
}


//Interact
void UInteractionComponent::TryInteract(EActionId ActionId, EInteractionType InteractionType)
{
	UpdateFocusActorAndLookedAtLocation();

	InteractionContext.FocusActor = FocusActor;
	InteractionContext.LookedAtLocation = LookedAtLocation;
	
	if (ActionIdInUse != ActionId && ActionIdInUse != EActionId::None) return;
	
	switch (InteractionType)
	{
	case EInteractionType::Started:
		ActionIdInUse = ActionId;
		break;
	case EInteractionType::Ongoing:
		break;
	case EInteractionType::Completed:
		ActionIdInUse = EActionId::None;
		break;
	}
	
	Server_TryInteract(ActionId, InteractionType, InteractionContext, FocusActor, CurrentDeltaTime);
}

void UInteractionComponent::Server_TryInteract_Implementation(EActionId ActionId, EInteractionType InteractionType,
	FInteractionContext Context, AActor* inFocusActor, float DeltaTime)
{
	//todo: refactor this (slightly)
	
	if (InteractionType == EInteractionType::Started)
	{
		if (FocusActorInteract(ActionId, InteractionType, Context, inFocusActor, DeltaTime)) return;
		if (HeldItemInteract(ActionId, InteractionType, Context, DeltaTime)) return;
	}
	else if (InteractionType == EInteractionType::Ongoing)
	{
		if (!Server_InteractionActor) return;
		IInteractable::Execute_InteractOngoing(Server_InteractionActor, ActionId, Context);
	}
	else if (InteractionType == EInteractionType::Completed)
	{
		if (!Server_InteractionActor) return;
		IInteractable::Execute_InteractCompleted(Server_InteractionActor, ActionId, Context);
		Server_InteractionActor = nullptr;
	}
}

bool UInteractionComponent::FocusActorInteract(EActionId ActionId, EInteractionType InteractionType,
	FInteractionContext Context, AActor* inFocusActor, float DeltaTime)
{
	if (!inFocusActor || !inFocusActor->GetClass()->ImplementsInterface(UInteractable::StaticClass())) return false;
	
	Server_InteractionActor = inFocusActor;

	return IInteractable::Execute_InteractStarted(inFocusActor, ActionId, Context);
}

bool UInteractionComponent::HeldItemInteract(EActionId ActionId, EInteractionType InteractionType,
	FInteractionContext Context, float DeltaTime)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent) return false;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return false;
	
	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return false;
	
	if (!HeldItemActor->GetClass()->ImplementsInterface(UInteractable::StaticClass())) return false;
	
	
	Server_InteractionActor = HeldItemActor;
	
	
	return IInteractable::Execute_InteractStarted(HeldItemActor, ActionId, Context);
}


//Getters
FVector UInteractionComponent::GetLookedAtLocation()
{
	UpdateFocusActorAndLookedAtLocation();
	
	return LookedAtLocation;
}