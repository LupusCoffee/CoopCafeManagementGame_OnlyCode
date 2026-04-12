#include "Systems/GamblerMacMinigame/GameAgent.h"
#include "Blueprint/UserWidget.h"
#include "Systems/GamblerMacMinigame/CardGameUI.h"
#include "Systems/GamblerMacMinigame/GamblerMacAiController.h"
#include "Systems/GamblerMacMinigame/TurnBasedSession.h"

//Setup
void UGameAgent::Setup(
	AActor* InActor, AController* InController, FTransform InPlayerSeat,
    UTurnBasedSession* InSession, TSubclassOf<UCardGameUI> InCardWidget, FName InName)
{
	// actor, controller, etc.
	Actor = InActor;

	HumanController = Cast<APlayerController>(InController);
	bIsAI = false;
	if (!HumanController)
	{
		AGamblerMacAiController* GamblerAiController = GetWorld()->SpawnActor<AGamblerMacAiController>(AGamblerMacAiController::StaticClass());
		GamblerAiController->Setup(InSession, this);
		AiController = GamblerAiController;
		bIsAI = true;
	}

	PlayerSeat = InPlayerSeat;

	AgentName = InName;


	// ui
	if (!HumanController) return;
	
	CardGameUIWidget = CreateWidget<UCardGameUI>(HumanController, InCardWidget);
	if (CardGameUIWidget) CardGameUIWidget->Setup(InSession, this);
}


//Tick
void UGameAgent::Tick(float DeltaTime)
{
	TurnTimeLeft -= DeltaTime;

	if (AiController) AiController->Tick(DeltaTime);	//only ticks if it's its turn --> this is a bit scuffed. the controller should be the one ticking the player, not the other way around......... DAMMN ITTTTTT AHHHGHGHGGGGHHHH
}


//Setters
// input
void UGameAgent::SetTurn(bool bMyTurn)
{
	if (bIsAI)
	{
		if (AiController) AiController->SetTurn(bMyTurn);
		return;
	}

	//is player...
	CardGameUIWidget->SetTurn(bMyTurn);
}

// points
void UGameAgent::SetPoints(int Value)
{
	TotalPoints = Value;
}

void UGameAgent::AddPoints(int Value)
{
	TotalPoints += Value;
}

void UGameAgent::RemovePoints(int Value)
{
	TotalPoints -= Value;
	if (TotalPoints < 0) TotalPoints = 0;
}

// cards
void UGameAgent::AddCardToHand(UBaseCard* Card)
{
	CardsInHand.Add(Card);
	
	if (CardGameUIWidget) CardGameUIWidget->UpdateCards();
}

void UGameAgent::RemoveCardFromHand(UBaseCard* Card)
{
	CardsInHand.Remove(Card);

	if (CardGameUIWidget) CardGameUIWidget->UpdateCards();
}

// context
void UGameAgent::SetTurnTime(float Value)
{
	TurnTimeLeft = Value;
}

void UGameAgent::SetPlayedCard(bool Value)
{
	bPlayedCard = Value;
}

// other ui
void UGameAgent::DisplayWinner(UGameAgent* Winner)
{
	if (CardGameUIWidget) CardGameUIWidget->DisplayWinner(Winner, this);
}


//Getters
// general
AActor* UGameAgent::GetActor()
{
	return Actor;
}

AController* UGameAgent::GetAIController()
{
	return AiController;
}

APlayerController* UGameAgent::GetHumanController()
{
	return HumanController;
}

bool UGameAgent::IsAI()
{
	return bIsAI;
}

FTransform UGameAgent::GetSeat()
{
	return PlayerSeat;
}

UUserWidget* UGameAgent::GetUI()
{
	return CardGameUIWidget;
}

FName UGameAgent::GetAgentName()
{
	return AgentName;
}

int UGameAgent::GetTotalPoints()
{
	return TotalPoints;
}

TArray<UBaseCard*> UGameAgent::GetCardsInHand() const
{
	return CardsInHand;
}

// context
float UGameAgent::GetTurnTime()
{
	return TurnTimeLeft;
}

bool UGameAgent::GetPlayedCard()
{
	return bPlayedCard;
}
