#include "Systems/GamblerMacMinigame/CardGameUI.h"
#include "Systems/GamblerMacMinigame/CardUI.h"
#include "Systems/GamblerMacMinigame/TurnBasedSession.h"

void UCardGameUI::Setup(UTurnBasedSession* InSession, UGameAgent* InPlayer)
{
	Session = InSession;
	Player = InPlayer;
}

void UCardGameUI::UpdateCards()
{
	OnUpdateCards.Broadcast(Player->GetCardsInHand(), bMyTurn);
}

void UCardGameUI::SetTurn(bool Value)
{
	bMyTurn = Value;
	
	OnUpdateTurnStatus.Broadcast(Value);
}

void UCardGameUI::DisplayWinner(UGameAgent* Winner, UGameAgent* Me)
{
	OnDisplayWinner.Broadcast(Winner, Me);
}

UTurnBasedSession* UCardGameUI::GetSession()
{
	return Session;
}

UGameAgent* UCardGameUI::GetPlayer()
{
	return Player;
}
