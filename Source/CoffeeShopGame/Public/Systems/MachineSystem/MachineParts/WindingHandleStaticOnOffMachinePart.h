#pragma once

#include "CoreMinimal.h"
#include "BaseWindingHandleMachinePart.h"
#include "WindingHandleStaticOnOffMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AWindingHandleStaticOnOffMachinePart : public ABaseWindingHandleMachinePart
{
	GENERATED_BODY()

public:
	AWindingHandleStaticOnOffMachinePart();

protected:
	virtual bool Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
	virtual bool Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext Context) override;
};
