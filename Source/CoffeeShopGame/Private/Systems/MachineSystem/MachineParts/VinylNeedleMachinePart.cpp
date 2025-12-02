#include "Systems/MachineSystem/MachineParts/VinylNeedleMachinePart.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Systems/Interaction System/Components/HighlightComponent.h"
#include "Systems/Interaction System/Components/InteractionComponent.h"
#include "Systems/Items/Components/MusicPlayerComponent.h"


//Setup and Tick
AVinylNeedleMachinePart::AVinylNeedleMachinePart()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVinylNeedleMachinePart::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetIsReplicated(true);
}

bool AVinylNeedleMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;

	VinylPlayerOwnerMachine = Cast<AVinylPlayerMachine>(OwnerMachine);
	if (!VinylPlayerOwnerMachine) return false;
	
	return true;
}


//Interface and Such
void AVinylNeedleMachinePart::Hover_Implementation(FInteractionContext Context)
{
	Super::Hover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->EnableHighlight();
}

void AVinylNeedleMachinePart::Unhover_Implementation(FInteractionContext Context)
{
	Super::Unhover_Implementation(Context);

	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}

bool AVinylNeedleMachinePart::InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	Super::InteractStarted_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;
	
	if (!Context.MovementComponent) return false;
	Context.MovementComponent->SetMovementMode(MOVE_None);
	
	return true;
}

bool AVinylNeedleMachinePart::InteractOngoing_Implementation(EActionId ActionId, FInteractionContext Context)
{
	Super::InteractOngoing_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;

	UpdateRotation(Context.LookedAtLocation);

	return true;
}

bool AVinylNeedleMachinePart::InteractCompleted_Implementation(EActionId ActionId, FInteractionContext Context)
{
	Super::InteractCompleted_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;

	if (!Context.MovementComponent) return false;
	Context.MovementComponent->SetMovementMode(MOVE_Walking);

	UpdateRotation(Context.LookedAtLocation);

	UpdateMusicBasedOnNeedleRotation();
	
	return true;
}

void AVinylNeedleMachinePart::UpdateRotation(FVector LookedAtLocation)
{
	if (LookedAtLocation == FVector::ZeroVector) return;
	
	//move rotation based on closest looked at location --> quite janky, fix later!!!
	FVector TargetLocation = FVector(LookedAtLocation.X, LookedAtLocation.Y, GetActorLocation().Z);

	FVector TargetDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
	FRotator ResultRotator = TargetDirection.Rotation();
	
	MeshComp->SetRelativeRotation(ResultRotator);

	CurrentRotationDegrees = FRotator::ClampAxis(ResultRotator.Yaw);
}

void AVinylNeedleMachinePart::UpdateMusicBasedOnNeedleRotation()
{
	if (!VinylPlayerOwnerMachine) return;

	float MinStartDegrees = VinylPlayerOwnerMachine->GetMinDegreeMusicStartRange();
	float MaxStartDegrees = VinylPlayerOwnerMachine->GetMaxDegreeMusicStartRange();
	
	if (MinStartDegrees < CurrentRotationDegrees && CurrentRotationDegrees < MaxStartDegrees)
	{
		VinylPlayerOwnerMachine->TryStartMusic();
	}
	else
	{
		VinylPlayerOwnerMachine->TryEndMusic();
	}
}