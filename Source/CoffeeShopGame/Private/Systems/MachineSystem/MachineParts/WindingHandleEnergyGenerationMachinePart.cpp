#include "Systems/MachineSystem/MachineParts/WindingHandleEnergyGenerationMachinePart.h"


//Setup
AWindingHandleEnergyGenerationMachinePart::AWindingHandleEnergyGenerationMachinePart()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ContainableComponent = CreateDefaultSubobject<UContainableComponent>("ContainableComponent");
}

void AWindingHandleEnergyGenerationMachinePart::BeginPlay()
{
	Super::BeginPlay();
}

bool AWindingHandleEnergyGenerationMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;

	//Container Component
	if (!ContainableComponent) return false;
	
	ContainableComponent->OnContainerEmpty.AddDynamic(this, &AWindingHandleEnergyGenerationMachinePart::TurnOffMachine);
	ContainableComponent->OnContainerNotEmpty.AddDynamic(this, &AWindingHandleEnergyGenerationMachinePart::TurnOnMachine);
	
	if (ContainableComponent->GetCurrentTotalVolume() > 0.0f) TurnOnMachine();
	
	return true;
}


//Machine On/Off
void AWindingHandleEnergyGenerationMachinePart::TurnOnMachine()
{
	OwnerMachine->TurnOn();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Turn On");
}

void AWindingHandleEnergyGenerationMachinePart::TurnOffMachine()
{
	OwnerMachine->TurnOff();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Turn Off");
}


//tick
float AWindingHandleEnergyGenerationMachinePart::RotateHandleTick(float DeltaTime)
{
	float RotationAmount = Super::RotateHandleTick(DeltaTime * EnergyGenerationRate);	//kinda becomes ambiguous --> fix

	//should add volume only if we move the handle
	//check how many degrees it moves?

	if (!ContainableComponent) return RotationAmount;
	
	float ExcessAmount = 0.0f;
	ContainableComponent->TryAddVolume(EResourceType::Energy, RotationAmount, ExcessAmount, true); //note: probably should use a variable for energy

	return RotationAmount;
}