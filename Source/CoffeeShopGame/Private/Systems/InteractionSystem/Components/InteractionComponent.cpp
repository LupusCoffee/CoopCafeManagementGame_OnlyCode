#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/InteractionComponent.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "CoffeeShopGame/Public/Systems/BuildingSystem/Components/BuildingComponent.h"
#include "CoffeeShopGame/Public/Systems/HolderSystem/HolderComponent/HolderComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PlayerPromptComponent.h"
class UHighlightRegistrySubsystem;


//Setup
UInteractionComponent::UInteractionComponent(): PlayerContext()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//Subsystems
	LookTraceSubsystem = GetWorld()->GetSubsystem<ULookTraceSubsystem>();

	//Controller
	CachedPawn = Cast<APawn>(GetOwner());
	if (!CachedPawn) return;

	//Setup IntervalTick & PersistentContext
	SetupIntervalTick();
	SetupPersistentContext();
	
	//Make sure controller is setup
	GetWorld()->GetTimerManager().SetTimer(ControllerSetupTimerHandle, this, 
		&UInteractionComponent::HandleControllerForPersistentContext,0.1f, true);
}

void UInteractionComponent::SetupIntervalTick()
{
	if (UpdateFocusActorFrequency <= 0.0f) return;
	
	FTimerHandle FocusTimer;
	const float Interval = 1.f / UpdateFocusActorFrequency;
	GetWorld()->GetTimerManager().SetTimer(FocusTimer, this, &UInteractionComponent::IntervalTick, Interval, true);
}


//Tick
void UInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//tick hover --> make into function
	if (HoveringActor) IInteractable::Execute_Local_TickHover(HoveringActor, PlayerContext, DeltaTime);
	for (UActorComponent* HoveredComp : HoveredComponents)
	{
		if (HoveredComp) IInteractable::Execute_Local_TickHover(HoveredComp, PlayerContext, DeltaTime);
	}
	
	TryTickInteraction(DeltaTime);
}

void UInteractionComponent::IntervalTick()
{
	RefreshDynamicContext();
	UpdateHover();
}


//Context
void UInteractionComponent::SetupPersistentContext()
{
	PlayerContext.Interactor = Cast<APlayerCharacter>(GetOwner());
	PlayerContext.InteractorController = CachedPawn->GetController();

	PlayerContext.InteractionComponent = this;
	PlayerContext.HolderComponent = GetOwner()->GetComponentByClass<UHolderComponent>();
	PlayerContext.MovementComponent = GetOwner()->GetComponentByClass<UCharacterMovementComponent>();
	
	PlayerContext.StatHandlerComponent = GetOwner()->GetComponentByClass<UStatHandlerComponent>();
	PlayerContext.BuildingComponent = GetOwner()->GetComponentByClass<UBuildingComponent>();
	
	PlayerContext.PlayerPromptComponent = GetOwner()->GetComponentByClass<UPlayerPromptComponent>();
}

void UInteractionComponent::HandleControllerForPersistentContext()
{
	if (PlayerContext.InteractorController)
	{
		GetWorld()->GetTimerManager().ClearTimer(ControllerSetupTimerHandle);
		return;
	}
	
	PlayerContext.InteractorController = CachedPawn->GetController();
}

FPlayerContext UInteractionComponent::RefreshDynamicContext()
{
	if (!LookTraceSubsystem || !CachedPawn || !CachedPawn->GetController()) return PlayerContext;
	
	FHitResult Hit = LookTraceSubsystem->GetHitResultFromControllerCameraSphereTrace(CachedPawn->GetController());
	
	PlayerContext.LookedAtLocation = Hit.Location;
	PlayerContext.LookedAtActor = Hit.GetActor();

	return PlayerContext;
}


//Hover
void UInteractionComponent::UpdateHover()
{
	//reset
	AActor* LookedAt = PlayerContext.LookedAtActor;
	AActor* NewHoveredActor = nullptr;
	TArray<UActorComponent*> NewHoveredComponents;
	
	//set new hovered actor and components
	if (LookedAt)
	{
		if (LookedAt->GetClass()->ImplementsInterface(UInteractable::StaticClass())) NewHoveredActor = LookedAt;
		
		for (UActorComponent* Component : LookedAt->GetComponents())
		{
			if (Component->GetClass()->ImplementsInterface(UInteractable::StaticClass())) NewHoveredComponents.Add(Component);
		}
	}
	
	//component
	if (HoveredComponents != NewHoveredComponents)
	{
		for (UActorComponent* OldHoveredComp : HoveredComponents)
		{
			if (OldHoveredComp) IInteractable::Execute_Local_EndHover(OldHoveredComp, PlayerContext);
		}
		for (UActorComponent* NewHoveredComp : NewHoveredComponents)
		{
			if (NewHoveredComp) IInteractable::Execute_Local_StartHover(NewHoveredComp, PlayerContext);
		}
		HoveredComponents = NewHoveredComponents;
	}
	
	//actor
	if (NewHoveredActor == HoveringActor) return;
	if (HoveringActor) IInteractable::Execute_Local_EndHover(HoveringActor, PlayerContext);
	if (NewHoveredActor) IInteractable::Execute_Local_StartHover(NewHoveredActor, PlayerContext);
	HoveringActor = NewHoveredActor;
}


//Interact
void UInteractionComponent::TryInteract(EActionId ActionId, EInteractionType InteractionType)
{	
	switch (InteractionType)
	{
		case EInteractionType::Started:	  TryStartInteraction(ActionId); break;
		case EInteractionType::Completed: TryEndInteraction(); break;
	}
}

// start interaction
void UInteractionComponent::TryStartInteraction(EActionId ActionId)
{
	Local_TryStartInteraction(ActionId);
	Server_TryStartInteraction(ActionId);
}

void UInteractionComponent::Local_TryStartInteraction(EActionId ActionId)
{
	if (ActiveInteraction.ActionId != ActionId && ActiveInteraction.ActionId != EActionId::None) return;
	ActiveInteraction.ActionId = ActionId;

	//todo: implement local interaction
}

void UInteractionComponent::Server_TryStartInteraction_Implementation(EActionId ActionId)
{
	// if active and different action --> return
	if (ActiveInteraction.IsActive() && ActiveInteraction.ActionId != ActionId) return;
	
	// interaction on ActiveActor + ActiveComponents
	const FPlayerContext& Context = RefreshDynamicContext();
	
	AActor* ActiveActor;
	TArray<UActorComponent*> ActiveComponents;
	Server_ResolveInteraction(ActionId, Context, ActiveActor, ActiveComponents);
	
	ActiveInteraction.Set(ActionId, ActiveActor, ActiveComponents);
}

void UInteractionComponent::Server_ResolveInteraction(EActionId ActionId, FPlayerContext Context, 
	AActor*& OutActor, TArray<UActorComponent*>& OutComponents)
{
	OutActor = nullptr;
	OutComponents.Empty();
	
	if (AActor* LookedAt = Context.LookedAtActor)
	{
		//note: executes interactable interface on actor and components
		bool bSuccess = Server_TryResolveInteractionGroup(LookedAt, ActionId, Context, OutActor, OutComponents);
		if (bSuccess) return;
	}
	
	if (AActor* Held = GetHeldInteractable(Context.HolderComponent))
	{
		//note: executes interactable interface on actor and components
		bool bSuccess = Server_TryResolveInteractionGroup(Held, ActionId, Context, OutActor, OutComponents);
		if (bSuccess) return;
	}
}

bool UInteractionComponent::Server_TryResolveInteractionGroup(AActor* ActorToResolve,
	EActionId ActionId, FPlayerContext Context, AActor*& OutActor, TArray<UActorComponent*>& OutComponents)
{
	// initial null check
	if (!ActorToResolve) return false;
	
	
	// set defaults
	OutActor = nullptr;
	OutComponents.Empty();
	
	
	// resolve just actor
	bool bActorSuccess = false;
	if (ActorToResolve->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		bActorSuccess = IInteractable::Execute_Server_StartInteraction(ActorToResolve, ActionId, Context);
		if (bActorSuccess) OutActor = ActorToResolve;
	}
	
	
	// resolve its components
	TArray<UActorComponent*> Components;
	ActorToResolve->GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (!Component->GetClass()->ImplementsInterface(UInteractable::StaticClass())) continue;
		
		bool bSuccess = IInteractable::Execute_Server_StartInteraction(Component, ActionId, Context);
		if (bSuccess) OutComponents.Add(Component);
	}
	
	bool bComponentsSuccess = OutComponents.Num() > 0;
	
	
	// return
	return bActorSuccess || bComponentsSuccess;
}

// tick interaction - runs on both client and server
void UInteractionComponent::TryTickInteraction(float DeltaTime)
{
	if (!ActiveInteraction.IsActive()) return;
	
	const FPlayerContext& Context = RefreshDynamicContext();
	
	if (!GetOwner()->HasAuthority())
	{		
		if (ActiveInteraction.Actor) IInteractable::Execute_Local_TickInteraction(ActiveInteraction.Actor, ActiveInteraction.ActionId, Context, DeltaTime);
		for (auto ActiveComp : ActiveInteraction.Components)
		{
			IInteractable::Execute_Local_TickInteraction(ActiveComp, ActiveInteraction.ActionId, Context, DeltaTime);
		}
	}
	else
	{
		if (ActiveInteraction.Actor) IInteractable::Execute_Server_TickInteraction(ActiveInteraction.Actor, ActiveInteraction.ActionId, Context, DeltaTime);
		for (auto ActiveComp : ActiveInteraction.Components)
		{
			IInteractable::Execute_Server_TickInteraction(ActiveComp, ActiveInteraction.ActionId, Context, DeltaTime);
		}
	}
}

// end interaction
void UInteractionComponent::TryEndInteraction()
{
	Local_TryEndInteraction();		//get interaction payload from this
	Server_TryEndInteraction();		//send in interaction payload in here
}

void UInteractionComponent::Local_TryEndInteraction()
{
	if (!ActiveInteraction.IsActive()) return;

	const FPlayerContext& Context = RefreshDynamicContext();
	
	if (ActiveInteraction.Actor) IInteractable::Execute_Local_EndInteraction(ActiveInteraction.Actor, ActiveInteraction.ActionId, Context);
	for (auto ActiveComp : ActiveInteraction.Components)
	{
		IInteractable::Execute_Local_EndInteraction(ActiveComp, ActiveInteraction.ActionId, Context);
	}
	
	if (GetWorld()->GetNetMode() != NM_Standalone) ActiveInteraction.Clear();
}

void UInteractionComponent::Server_TryEndInteraction_Implementation()
{
	if (!ActiveInteraction.IsActive()) return;

	const FPlayerContext& Context = RefreshDynamicContext();
	
	if (ActiveInteraction.Actor) IInteractable::Execute_Server_EndInteraction(ActiveInteraction.Actor, ActiveInteraction.ActionId, Context);
	for (auto ActiveComp : ActiveInteraction.Components)
	{
		IInteractable::Execute_Server_EndInteraction(ActiveComp, ActiveInteraction.ActionId, Context);
	}
	
	ActiveInteraction.Clear();
}


//Helpers
AActor* UInteractionComponent::GetHeldInteractable(UHolderComponent* HolderComponent)
{
	if (!HolderComponent) return nullptr;
	
	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem) return nullptr;
	
	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return nullptr;
	
	return HeldItemActor;
}


//Getters
FVector UInteractionComponent::GetLookedAtLocation()
{
	const FPlayerContext& Context = RefreshDynamicContext();
	
	return Context.LookedAtLocation;
}