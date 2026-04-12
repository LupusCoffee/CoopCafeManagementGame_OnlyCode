#include "Systems/GamblerMacMinigame/TurnBasedManager.h"
#include "Systems/ActionStack/ActionStack.h"
#include "Systems/GamblerMacMinigame/TurnBasedSession.h"

void UTurnBasedManager::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World) return;

	DeltaTime = World->GetDeltaSeconds();
	
	for (int32 i = GameSessions.Num() - 1; i >= 0; --i)
	{
		UTurnBasedSession* Session = GameSessions[i];
		if (!Session)
		{
			GameSessions.RemoveAtSwap(i);
			continue;
		}

		Session->Tick(DeltaTime);
		
		UActionStack* ActionStack = Session->GetActionStack();
		if (!ActionStack ||ActionStack->IsEmpty()) GameSessions.RemoveAtSwap(i);
	}
}

//if a controller is left blank, it will get an ai controller instead
UTurnBasedSession* UTurnBasedManager::StartGame(
	AActor* InPlayer1, AController* InController1,
	AActor* InPlayer2, AController* InController2,
	FTransform InPlayerSeat1, FTransform InPlayerSeat2,
	TSubclassOf<UCardGameUI> InCardWidget, UCardGroupData* InDrawPile,
	UTurnOrder* InTurnOrderData, float InTurnTime)	//todo: make this more generic
{
	UTurnBasedSession* NewSession = NewObject<UTurnBasedSession>(this);
	GameSessions.Add(NewSession);
	
	NewSession->SetupSession(InPlayer1, InController1, InPlayer2, InController2,
		InPlayerSeat1, InPlayerSeat2, InCardWidget, InDrawPile, InTurnOrderData,
		InTurnTime);
	
	return NewSession;
}
