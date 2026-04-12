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
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
};
