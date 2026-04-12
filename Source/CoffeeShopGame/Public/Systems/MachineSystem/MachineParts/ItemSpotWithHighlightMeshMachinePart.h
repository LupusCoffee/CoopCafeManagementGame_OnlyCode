#pragma once

#include "CoreMinimal.h"
#include "ItemSpotMachinePart.h"
#include "UObject/Object.h"
#include "ItemSpotWithHighlightMeshMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AItemSpotWithHighlightMeshMachinePart : public AItemSpotMachinePart
{
	GENERATED_BODY()
	
public:
	virtual bool Init(AMachine* _Owner, FName _ParentSocket) override;
	
protected:
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
	
	UFUNCTION()
	virtual void SetHighlightMesh(UHeldItem* ItemToHighlight);
};
