#include "Systems/ActionStack/Actions/NewGameAction.h"
#include "Systems/ActionStack/Actions/TurnChooserAction.h"
#include "Systems/GamblerMacMinigame/TurnBasedSessionContext.h"

void UNewGameAction::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	
	if (bFirstTime)
	{
		bIsDone = false;
		
		GameSession = GetActionStackContext<UTurnBasedSessionContext>()->Session;
		
		GameSession->StartGame();

		GameSession->GetActionStack()->PushAction(UTurnChooserAction::StaticClass());
	}
	else
	{
		GameSession->DisablePlayers();
	}
}

void UNewGameAction::OnTick_Implementation(float DeltaTime)
{
	Super::OnTick_Implementation(DeltaTime);

	bIsDone = GameSession->GetGameIsDone();
}

void UNewGameAction::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
}

bool UNewGameAction::IsDone_Implementation() const
{
	return bIsDone;
}
