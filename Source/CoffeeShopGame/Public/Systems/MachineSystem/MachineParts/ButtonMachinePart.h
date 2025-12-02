#pragma once

#include "CoreMinimal.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "ButtonMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AButtonMachinePart : public AMachinePart
{
	GENERATED_BODY()

public:
	AButtonMachinePart();

protected:
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;
};
