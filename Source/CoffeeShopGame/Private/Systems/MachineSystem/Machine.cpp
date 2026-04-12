#include "Systems/MachineSystem/Machine.h"
#include "Net/UnrealNetwork.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/HighlightComponent.h"
#include "Systems/MachineSystem/MachinePart.h"


//Constructor, BeginPlay, etc.
AMachine::AMachine()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
	
	RootComponent = MeshComp;

	//Replication
	bReplicateUsingRegisteredSubObjectList = true;
}

void AMachine::BeginPlay()
{
	Super::BeginPlay();

	SetupMachineParts();
}


//Methods
void AMachine::SetupMachineParts()
{
	if (!HasAuthority()) return;
	
	for (auto Element : MachineParts)
	{
		FName SocketName = Element.Key;
		TSubclassOf<AMachinePart> MachinePartClass = Element.Value;
		if (!MachinePartClass) continue;

		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AMachinePart* MachinePart = GetWorld()->SpawnActor<AMachinePart>(MachinePartClass, Params);
		if (!MachinePart) continue;
		
		MachinePart->Init(this, SocketName);
		MachinePart->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

		InstancedMachineParts.Add(MachinePart);
	}
	
	OnMachineSetup.Broadcast(InstancedMachineParts);
}

void AMachine::TurnOn()
{
	bIsOn = true;
	
	OnMachineActivated.Broadcast();
}

void AMachine::TurnOff()
{
	bIsOn = false;
	
	OnMachineDeactivated.Broadcast();
}

bool AMachine::IsOn()
{
	return bIsOn;
}

TArray<AMachinePart*> AMachine::GetMachineParts()
{
	return InstancedMachineParts;
}

UStaticMeshComponent* AMachine::GetMeshComponent()
{
	return MeshComp;
}


//Interfaces
void AMachine::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachine, bIsOn);
}

void AMachine::Local_StartHover_Implementation(FPlayerContext Context)
{
	if (!HighlightComponent) return;
	if (!Context.HolderComponent) return;
	if (!Context.HolderComponent->GetHeldItem()) return;

	AActor* HeldActor = Context.HolderComponent->GetHeldItem()->GetActor();
	if (!HeldActor) return;

	for (auto MachinePart : InstancedMachineParts)
	{
		if (HeldActor == MachinePart)
		{
			HighlightComponent->EnableHighlight();
			return;
		}
	}
}

void AMachine::Local_EndHover_Implementation(FPlayerContext Context)
{
	if (!HighlightComponent) return;
	HighlightComponent->DisableHighlight();
}