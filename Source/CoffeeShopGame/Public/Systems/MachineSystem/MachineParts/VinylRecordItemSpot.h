#pragma once

#include "CoreMinimal.h"
#include "ItemSpotMachinePart.h"
#include "Systems/Items/Actors/VinylRecord.h"
#include "Systems/StatModificationSystem/Components/StatModApplicationComponent.h"
#include "VinylRecordItemSpot.generated.h"
class AVinylPlayerMachine;
class UMusicPlayerComponent;

UCLASS()
class COFFEESHOPGAME_API AVinylRecordItemSpot : public AItemSpotMachinePart
{
	GENERATED_BODY()

public:
	AVinylRecordItemSpot();
	virtual bool Init(AMachine* _Owner, FName _ParentSocket) override;

protected:
	//Variables --> Visible, Machine and Components
	UPROPERTY(BlueprintReadOnly)
	AVinylPlayerMachine* VinylPlayerOwnerMachine = nullptr;
	
	//Variables, Hidden --> Dynamic
	UPROPERTY(BlueprintReadOnly)
	AVinylRecord* VinylRecordAtSpot = nullptr;

	//Methods --> Overrides
	virtual bool PlaceItem(UHolderComponent* HolderComponent) override;
	virtual bool TakeItem(UHolderComponent* HolderComponent) override;
};
