#include "Systems/GamblerMacMinigame/GamblerMacAiController.h"

#include "Systems/GamblerMacMinigame/TurnBasedSession.h"

AGamblerMacAiController::AGamblerMacAiController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGamblerMacAiController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGamblerMacAiController::Setup(UTurnBasedSession* InSession, UGameAgent* InAgent)
{
	Session = InSession;
	Agent = InAgent;
	
	WaitTimeBeforeChoosingCard = FMath::RandRange(MinTime, MaxTime);
	CurrentTime = 0;
}

void AGamblerMacAiController::SetTurn(bool bInMyTurn)
{
	bMyTurn = bInMyTurn;
}

void AGamblerMacAiController::Tick(float DeltaTime)
{
	if (!bMyTurn) return;
	
	Super::Tick(DeltaTime);

	if (CurrentTime > WaitTimeBeforeChoosingCard)
	{
		TArray<UBaseCard*> Cards = Agent->GetCardsInHand();
		UBaseCard* RandomCard = Cards[rand() % Cards.Num()];
		Session->PlayCard(RandomCard, Agent);

		WaitTimeBeforeChoosingCard = FMath::RandRange(MinTime, MaxTime);
		CurrentTime = 0;
	}
	else CurrentTime += DeltaTime;
}

