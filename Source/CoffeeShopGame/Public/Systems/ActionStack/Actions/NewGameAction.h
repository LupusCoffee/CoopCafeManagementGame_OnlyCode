#pragma once

#include "CoreMinimal.h"
#include "Systems/ActionStack/BaseAction.h"
#include "Systems/GamblerMacMinigame/TurnBasedSession.h"
#include "NewGameAction.generated.h"

UCLASS()
class COFFEESHOPGAME_API UNewGameAction : public UBaseAction
{
	GENERATED_BODY()

public:
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	virtual void OnTick_Implementation(float DeltaTime) override;
	virtual void OnEnd_Implementation() override;
	virtual bool IsDone_Implementation() const override;

private:
	UPROPERTY()
	UTurnBasedSession* GameSession = nullptr;

	bool bIsDone = false;
};
