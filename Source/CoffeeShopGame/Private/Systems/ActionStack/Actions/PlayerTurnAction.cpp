#include "Systems/ActionStack/Actions/PlayerTurnAction.h"
#include "Systems/GamblerMacMinigame/TurnBasedSession.h"
#include "Systems/GamblerMacMinigame/TurnBasedSessionContext.h"

void UPlayerTurnAction::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);

	if (bFirstTime)
	{
		GameSession = GetActionStackContext<UTurnBasedSessionContext>()->Session;
		
		Player = GameSession->GetCurrentPlayer();

		float TurnTime = GameSession->GetTurnTime();
		Player->SetTurnTime(TurnTime);

		Player->SetPlayedCard(false);
	}
}

void UPlayerTurnAction::OnTick_Implementation(float DeltaTime)
{
	Super::OnTick_Implementation(DeltaTime);

	Player->Tick(DeltaTime);
}

bool UPlayerTurnAction::IsDone_Implementation() const
{
	return Player->GetTurnTime() <= 0 || Player->GetPlayedCard();	//sub to event instead?
}