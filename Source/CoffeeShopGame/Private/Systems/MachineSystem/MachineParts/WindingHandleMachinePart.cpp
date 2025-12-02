#include "Systems/MachineSystem/MachineParts/WindingHandleMachinePart.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Systems/Interaction System/Components/HighlightComponent.h"
#include "Systems/Items/Components/ContainerComponent.h"


//Setup and Tick
AWindingHandleMachinePart::AWindingHandleMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWindingHandleMachinePart::BeginPlay()
{
	Super::BeginPlay();
}

bool AWindingHandleMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;

	//Container Component
	ContainerComponent = OwnerMachine->GetComponentByClass<UContainerComponent>();
	if (!ContainerComponent) return false;
	
	ContainerComponent->OnContainerEmpty.AddDynamic(this, &AWindingHandleMachinePart::TurnOffMachine);
	ContainerComponent->OnContainerNonEmpty.AddDynamic(this, &AWindingHandleMachinePart::TurnOnMachine);
	
	if (ContainerComponent->GetCurrentTotalVolume() > 0.0f) TurnOnMachine();
	
	return true;
}

void AWindingHandleMachinePart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldTickInteraction) InteractionTick(DeltaTime);
}


//General Methods
void AWindingHandleMachinePart::TurnOnMachine()
{
	OwnerMachine->TurnOn();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Turn On");
}

void AWindingHandleMachinePart::TurnOffMachine()
{
	OwnerMachine->TurnOff();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Turn Off");
}


//Interface
void AWindingHandleMachinePart::Hover_Implementation(FInteractionContext Context)
{
	Super::Hover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void AWindingHandleMachinePart::Unhover_Implementation(FInteractionContext Context)
{
	Super::Unhover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}

bool AWindingHandleMachinePart::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	Super::InteractStarted_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;

	if (!Context.MovementComponent) return false;
	Context.MovementComponent->SetMovementMode(MOVE_None);

	bShouldTickInteraction = true;
	
	return true;
}

bool AWindingHandleMachinePart::InteractCompleted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	Super::InteractCompleted_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;

	if (!Context.MovementComponent) return false;
	Context.MovementComponent->SetMovementMode(MOVE_Walking);

	bShouldTickInteraction = false;
	
	return true;
}

void AWindingHandleMachinePart::InteractionTick(float DeltaTime)
{
	//should add volume only if we move the handle
	//check how many degrees it moves?

	float RotationAmount = EnergyGenerationRate * DeltaTime;
	if (MeshComp) MeshComp->AddLocalRotation(FRotator(0, RotationAmount * VisualHandleSpinRate, 0));

	ContainerComponent->TryAddVolume(EResourceType::Energy, RotationAmount);

	/*FRotator Rot = MeshComp ? MeshComp->GetRelativeRotation() : FRotator::ZeroRotator;
	FString Msg = FString::Printf(TEXT("Rot: %s  |  Pitch=%.2f  Yaw=%.2f  Roll=%.2f"), *Rot.ToCompactString(), Rot.Pitch, Rot.Yaw, Rot.Roll);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Msg);*/
}