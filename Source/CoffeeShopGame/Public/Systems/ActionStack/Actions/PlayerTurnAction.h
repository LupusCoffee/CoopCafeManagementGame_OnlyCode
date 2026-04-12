#pragma once

#include "CoreMinimal.h"
#include "Systems/ActionStack/BaseAction.h"
#include "Systems/GamblerMacMinigame/GameAgent.h"
#include "PlayerTurnAction.generated.h"
class UTurnBasedSession;

UCLASS()
class COFFEESHOPGAME_API UPlayerTurnAction : public UBaseAction
{
	GENERATED_BODY()

public:
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	virtual void OnTick_Implementation(float DeltaTime) override;
	virtual bool IsDone_Implementation() const override;

private:
	UPROPERTY()
	UTurnBasedSession* GameSession = nullptr;

	UPROPERTY()
	UGameAgent* Player = nullptr;
};
