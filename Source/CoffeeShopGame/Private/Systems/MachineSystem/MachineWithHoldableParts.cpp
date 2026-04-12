#include "Systems/MachineSystem/MachineWithHoldableParts.h"

AMachineWithHoldableParts::AMachineWithHoldableParts()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMachineWithHoldableParts::BeginPlay()
{
	Super::BeginPlay();

	for (auto MachinePart : InstancedMachineParts)
	{
		AHoldableMachinePart* HoldableMachinePart = Cast<AHoldableMachinePart>(MachinePart);
		if (HoldableMachinePart) HoldableMachineParts.Add(HoldableMachinePart);
	}
}

void AMachineWithHoldableParts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

