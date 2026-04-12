#include "Systems/MachineSystem/SpecificMachines/CoffeeGrinderMachine.h"
#include "NiagaraComponent.h"
#include "Systems/MachineSystem/MachineParts/ContainerViaOwnerMachinePart.h"
#include "Systems/MachineSystem/MachineParts/HoldableContainerViaOwnerMachinePart.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Subsystems/ResourceConverterSubsystem.h"


//Setup
ACoffeeGrinderMachine::ACoffeeGrinderMachine()
{
	PrimaryActorTick.bCanEverTick = true;

	PourVfxComp = CreateDefaultSubobject<UNiagaraComponent>("PourVFXComp");
	PourVfxComp->SetupAttachment(MeshComp);
}

void ACoffeeGrinderMachine::BeginPlay()
{
	Super::BeginPlay();
	
	//Components
	for (auto MachinePart : InstancedMachineParts)
	{
		if (!MachinePart) continue;
		if (Cast<AContainerViaOwnerMachinePart>(MachinePart)) PreGrindedContainer = MachinePart->GetComponentByClass<UContainableComponent>();
		if (Cast<AHoldableContainerViaOwnerMachinePart>(MachinePart)) PostGrindedContainer = MachinePart->GetComponentByClass<UContainableComponent>();
	}
	
	//Subsystems
	ResourceConverterSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UResourceConverterSubsystem>();
}


//Tick
void ACoffeeGrinderMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsOn()) TickGrindProgress(DeltaTime);
}

void ACoffeeGrinderMachine::TickGrindProgress(float DeltaTime)
{	
	if (!PreGrindedContainer || !PostGrindedContainer) return;
	
	ResourceConverterSubsystem->TryMoveContentAndGrind(GrindSpeed * DeltaTime,
		PreGrindedContainer->GetContainerObject(), PostGrindedContainer->GetContainerObject());
}


//Cosmetic
void ACoffeeGrinderMachine::TurnOn()
{
	Super::TurnOn();
	
	if (!PourVfxComp->IsActive()) PourVfxComp->Activate();
}

void ACoffeeGrinderMachine::TurnOff()
{
	Super::TurnOff();

	if (PourVfxComp->IsActive()) PourVfxComp->Deactivate();
}
