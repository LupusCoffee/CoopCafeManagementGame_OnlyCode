#include "Systems/ActionStack/Actions/TurnChooserAction.h"
#include "Systems/ActionStack/Actions/PlayerTurnAction.h"
#include "Systems/GamblerMacMinigame/TurnBasedSession.h"
#include "Systems/GamblerMacMinigame/TurnBasedSessionContext.h"

void UTurnChooserAction::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);

	if (bFirstTime)
	{
		GameSession = GetActionStackContext<UTurnBasedSessionContext>()->Session;
		
		bIsDone = false;
	}

	
	bIsDone = GameSession->TryEndRound();
	if (bIsDone) return;

	
	GameSession->ChoosePlayerTurn();
	GameSession->GetActionStack()->PushAction(UPlayerTurnAction::StaticClass());
}

bool UTurnChooserAction::IsDone_Implementation() const
{
	return bIsDone;
}
