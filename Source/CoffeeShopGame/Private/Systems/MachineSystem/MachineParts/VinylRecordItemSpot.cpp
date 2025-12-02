#include "Systems/MachineSystem/MachineParts/VinylRecordItemSpot.h"

#include "Systems/Items/Components/MusicPlayerComponent.h"
#include "Systems/MachineSystem/SpecificMachines/VinylPlayerMachine.h"


AVinylRecordItemSpot::AVinylRecordItemSpot()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AVinylRecordItemSpot::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;

	VinylPlayerOwnerMachine = Cast<AVinylPlayerMachine>(OwnerMachine);
	if (!VinylPlayerOwnerMachine) return false;
	
	return true;
}

bool AVinylRecordItemSpot::PlaceItem(UHolderComponent* HolderComponent)
{
	if (!Super::PlaceItem(HolderComponent)) return false;

	if (!VinylPlayerOwnerMachine) return false;
	
	VinylRecordAtSpot = Cast<AVinylRecord>(ItemAtSpot->GetActor());
	VinylPlayerOwnerMachine->SetVinylRecord(VinylRecordAtSpot);

	return true;
}

bool AVinylRecordItemSpot::TakeItem(UHolderComponent* HolderComponent)
{
	if (!Super::TakeItem(HolderComponent)) return false;

	if (!VinylPlayerOwnerMachine) return false;

	VinylRecordAtSpot = nullptr;
	VinylPlayerOwnerMachine->SetVinylRecord(VinylRecordAtSpot);

	return true;
}

