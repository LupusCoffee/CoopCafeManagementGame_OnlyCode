#include "Systems/MachineSystem/MachineParts/VinylNeedleMachinePart.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/InteractionComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionEnum.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"


//Setup and Tick
AVinylNeedleMachinePart::AVinylNeedleMachinePart()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVinylNeedleMachinePart::BeginPlay()
{
	Super::BeginPlay();

	BaseMeshComp->SetIsReplicated(true);
}

bool AVinylNeedleMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;

	VinylPlayerOwnerMachine = Cast<AVinylPlayerMachine>(OwnerMachine);
	if (!VinylPlayerOwnerMachine) return false;
	
	return true;
}


//Interface and Such
void AVinylNeedleMachinePart::Local_StartHover_Implementation(FPlayerContext Context)
{
	Super::Local_StartHover_Implementation(Context);

	if (HighlightComponent) HighlightComponent->EnableHighlight();
	
	if (!ItemPromptComp || !ItemPromptComp->GetPromptBox()) return;
	ItemPromptComp->GetPromptBox()->SetPrompts({EAction::MachineInteraction_Grab});
}

void AVinylNeedleMachinePart::Local_EndHover_Implementation(FPlayerContext Context)
{
	Super::Local_EndHover_Implementation(Context);

	if (HighlightComponent) HighlightComponent->DisableHighlight();
	
	if (!ItemPromptComp || !ItemPromptComp->GetPromptBox()) return;
	ItemPromptComp->GetPromptBox()->ClearPrompts();
}

bool AVinylNeedleMachinePart::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_StartInteraction_Implementation(ActionId, Context);

	if (ActionId != EActionId::LeftMouseButton) return false;
	
	if (!Context.MovementComponent) return false;
	Context.MovementComponent->SetMovementMode(MOVE_None);
	
	return true;
}

bool AVinylNeedleMachinePart::Server_TickInteraction_Implementation(EActionId ActionId, FPlayerContext Context, float DeltaTime)
{
	Super::Server_TickInteraction_Implementation(ActionId, Context, DeltaTime);

	if (ActionId != EActionId::LeftMouseButton) return false;

	UpdateRotation(Context.LookedAtLocation);

	return true;
}

bool AVinylNeedleMachinePart::Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context)
{
	Super::Server_EndInteraction_Implementation(ActionId, Context);

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
	FRotator ResultRotator = TargetDirection.Rotation() + FRotator(0, 90, 0);
	
	BaseMeshComp->SetWorldRotation(ResultRotator);

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