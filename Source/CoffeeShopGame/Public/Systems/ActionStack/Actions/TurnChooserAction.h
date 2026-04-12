#pragma once

#include "CoreMinimal.h"
#include "Systems/ActionStack/BaseAction.h"
#include "TurnChooserAction.generated.h"

class UTurnBasedSession;

UCLASS()
class COFFEESHOPGAME_API UTurnChooserAction : public UBaseAction
{
	GENERATED_BODY()

public:
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	virtual bool IsDone_Implementation() const override;

private:
	UPROPERTY()
	UTurnBasedSession* GameSession = nullptr;

	bool bIsDone = false;
};
