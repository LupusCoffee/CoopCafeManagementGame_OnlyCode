#include "Systems/Items/Actors/ContainerItem.h"
#include "Systems/Items/Components/ContainerComponent.h"
#include "Systems/StatModificationSystem/GameTags.h"


//Setup
AContainerItem::AContainerItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ContainerComp = CreateDefaultSubobject<UContainerComponent>(TEXT("ContainerComp"));

	PourVfxComp = CreateDefaultSubobject<UNiagaraComponent>("PourVFXComp");
	PourVfxComp->SetupAttachment(MeshComp);
}

void AContainerItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!ContainerComp) ContainerComp = GetComponentByClass<UContainerComponent>();
	if (!ContainerComp) return;
	
	bool SetupStatus = ContainerComp->SetupContainerData(ContainerData);
	if (!SetupStatus) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CoffeePowder Container Data Setup Incomplete, using default values instead."));
}

void AContainerItem::BeginPlay()
{
	Super::BeginPlay();

	OnPourStart.AddDynamic(this, &AContainerItem::ActivatePourVFX);
	OnPourEnd.AddDynamic(this, &AContainerItem::DeactivatePourVFX);
	ContainerComp->OnContainerEmpty.AddDynamic(this, &AContainerItem::DeactivatePourVFX);
}


//Tick
void AContainerItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsPouring) ContainerComp->PourDownTick(DeltaSeconds);
}


//Interface
bool AContainerItem::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (ActionId == EActionId::LeftMouseButton
		&& HolderComponent
		&& HolderComponent->GetHeldItem()
		&& HolderComponent->GetHeldItem()->GetActor() == this)
	{
		UStatHandlerComponent* StatHandler = Context.StatHandlerComponent;
		float PourRate = 0.0f;

		float ModifiedStatValue = 0.0f;
		if (StatHandler && StatHandler->TryGetModifiedPropertyStat(TAG_PropertyStat_Player_PourRate, ModifiedStatValue)) PourRate = ModifiedStatValue;
		
		return PourStart(HolderComponent, Context.FocusActor, Context.MovementComponent, PourRate);
	}
	
	return Super::InteractStarted_Implementation(ActionId, Context);
}

bool AContainerItem::InteractOngoing_Implementation(EActionId ActionId, FInteractionContext Context)
{
	UHolderComponent* HolderComponent = Context.HolderComponent;
	
	if (ActionId == EActionId::LeftMouseButton
		&& HolderComponent
		&& HolderComponent->GetHeldItem()
		&& HolderComponent->GetHeldItem()->GetActor() == this)
	{
		UStatHandlerComponent* StatHandler = Context.StatHandlerComponent;
		float PourRate = 0.0f;

		float ModifiedStatValue = 0.0f;
		if (StatHandler && StatHandler->TryGetModifiedPropertyStat(TAG_PropertyStat_Player_PourRate, ModifiedStatValue)) PourRate = ModifiedStatValue;
		
		//note: InteractOngoing is NOT on tick, therefore i only update the pour rate
		//used for the actual tick function, instead of ticking it down here directly
		return UpdatePourRateOngoing(PourRate);
	}
	
	return Super::InteractOngoing_Implementation(ActionId, Context);
}

bool AContainerItem::InteractCompleted_Implementation(EActionId ActionId, FInteractionContext InteractorContext)
{
	if (ActionId == EActionId::LeftMouseButton
		&& InteractorContext.HolderComponent
		&& InteractorContext.HolderComponent->GetHeldItem()
		&& InteractorContext.HolderComponent->GetHeldItem()->GetActor() == this)
	{
		return PourEnd(InteractorContext.HolderComponent, InteractorContext.MovementComponent);
	}
	
	return Super::InteractCompleted_Implementation(ActionId, InteractorContext);
}

void AContainerItem::Recieve_Implementation(EResourceType ResourceTypeToAdd, float FilledVolume)
{
	IFillable::Recieve_Implementation(ResourceTypeToAdd, FilledVolume);

	if (ContainerComp) ContainerComp->TryAddVolume(ResourceTypeToAdd, FilledVolume);
}


//Used by Interface
bool AContainerItem::PourStart(UHolderComponent* HolderCompForPourer, AActor* PourTargetActor,
	UCharacterMovementComponent* MovementComponent, float InPourRate)
{
	if (!HolderCompForPourer) return false;
	if (!MovementComponent) return false;
	if (!ContainerComp) return false;

	MovementComponent->SetMovementMode(MOVE_None);
	
	ContainerComp->PourIntoMovedOverTargetStart(HolderCompForPourer, PourTargetActor, InPourRate);
	
	bIsPouring = true;
	OnPourStart.Broadcast();
	
	return true;
}

bool AContainerItem::UpdatePourRateOngoing(float InPourRate)
{
	if (!ContainerComp) return false;
	
	ContainerComp->SetPourRate(InPourRate); //gotta do this if i want to update the pour rate over the duration of the pour --> what if the multiplier changes mid-pour!

	return true;
}

bool AContainerItem::PourEnd(UHolderComponent* HolderCompForPourer, UCharacterMovementComponent* MovementComponent)
{
	if (!HolderCompForPourer) return false;
	if (!MovementComponent) return false;
	
	bIsPouring = false;
	OnPourEnd.Broadcast();

	ContainerComp->PourIntoMovedOverTargetEnd(HolderCompForPourer);
	
	MovementComponent->SetMovementMode(MOVE_Walking);
	
	return true;
}


//Visuals
void AContainerItem::ActivatePourVFX_Implementation()
{
	if (!ContainerComp || ContainerComp->GetPouringResourceType() == EResourceType::None || ContainerComp->GetCurrentTotalVolume() <= 0.0f) return;
	
	PourVfxComp->Activate();

	InitialRotation = MeshComp->GetRelativeRotation();
	MeshComp->AddLocalRotation(FRotator(180.0f, 0.0f, 0.0f));
}

void AContainerItem::DeactivatePourVFX_Implementation()
{
	PourVfxComp->Deactivate();
	
	MeshComp->SetRelativeRotation(InitialRotation);
}