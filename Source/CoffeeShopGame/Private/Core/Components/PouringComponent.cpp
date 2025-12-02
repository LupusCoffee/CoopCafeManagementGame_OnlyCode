#include "Core/Components/PouringComponent.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

UPouringComponent::UPouringComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	PourVFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PourVFX"));
	PourVFXComp->bAutoActivate = false;
	PourVFXComp->SetAutoDestroy(false);

	//Replication
	bReplicateUsingRegisteredSubObjectList = true;
}

void UPouringComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (!OwnerMesh || !PourVFX || !PourVFXComp) return;

	PourVFXComp->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, PourSocket);
	PourVFXComp->SetAsset(PourVFX);
}

void UPouringComponent::StartVFX()
{
	if (!PourVFXComp || !PourVFX) return;

	ForRep_bPourVfxIsOn = true;
	
	PourVFXComp->Activate(true);
}

void UPouringComponent::StopVFX()
{
	if (!PourVFXComp) return;

	ForRep_bPourVfxIsOn = false;
	
	PourVFXComp->Deactivate();
}


//Replication
// onrep
void UPouringComponent::OnRep_IsPouring()
{
	if (IsNetMode(NM_DedicatedServer)) return; // never spawn FX on dedicated server

	if (!PourVFXComp || !PourVFX) return;
	PourVFXComp->SetAsset(PourVFX);

	if (ForRep_bPourVfxIsOn)	PourVFXComp->Activate(true);
	else						PourVFXComp->Deactivate();
}

// setup
void UPouringComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPouringComponent, ForRep_bPourVfxIsOn);
}