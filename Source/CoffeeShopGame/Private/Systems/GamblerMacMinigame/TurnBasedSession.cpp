#include "Systems/GamblerMacMinigame/TurnBasedSession.h"
#include "Blueprint/UserWidget.h"
#include "Systems/ActionStack/ActionStack.h"
#include "Systems/GamblerMacMinigame/CardGameUI.h"
#include "Systems/GamblerMacMinigame/TurnBasedSessionContext.h"
#include "Systems/GamblerMacMinigame/TurnOrder.h"


//Setup
void UTurnBasedSession::SetupSession(
	AActor* InPlayer1, AController* InController1,
	AActor* InPlayer2, AController* InController2,
	FTransform InPlayerSeat1, FTransform InPlayerSeat2,
	TSubclassOf<UCardGameUI> InCardWidget, UCardGroupData* InDrawPile,
	UTurnOrder* InTurnOrderData, float InTurnTime)
{
	//variable setup
	// setup players
	Player1 = NewObject<UGameAgent>(this);
	Player2 = NewObject<UGameAgent>(this);
	Player1->Setup(InPlayer1, InController1, InPlayerSeat1, this, InCardWidget, "Player1");
	Player2->Setup(InPlayer2, InController2, InPlayerSeat2, this, InCardWidget, "Player2");
	Player1InitialTransform = InPlayer1->GetTransform();
	Player2InitialTransform = InPlayer2->GetTransform();
	
	AllPlayers.Add(Player1);
	AllPlayers.Add(Player2);
	

	// setup card objects
	for (auto CardEntry : InDrawPile->Cards)
	{
		for (int i = 0; i < CardEntry.Amount; ++i)
		{
			UBaseCard* CardObject = NewObject<UBaseCard>(this);
			if (!CardObject) continue;

			CardObject->SetupData(CardEntry.CardData);
		
			DrawPile.Add(CardObject);
		}
	}

	// setup turn order
	TurnOrderData = InTurnOrderData;

	// setup turn time
	TurnTime = InTurnTime;

	
	//action stack setup
	ActionStack = NewObject<UActionStack>(this);
	UTurnBasedSessionContext* Context = NewObject<UTurnBasedSessionContext>(this);
	Context->Init(this);
	ActionStack->SetupAllActions(Context);
}


//Tick
void UTurnBasedSession::Tick(float DeltaTime)
{
	if (!ActionStack) return;

	ActionStack->Tick(DeltaTime);
}


//Getters
UActionStack* UTurnBasedSession::GetActionStack()
{
	return ActionStack;
}

UGameAgent* UTurnBasedSession::GetCurrentPlayer()
{
	return CurrentPlayer;
}

float UTurnBasedSession::GetTurnTime()
{
	return TurnTime;
}

bool UTurnBasedSession::GetGameIsDone()
{
	return bGameIsDone;
}


//Action Functions
// setup stuff
void UTurnBasedSession::StartGame()
{
	SetIgnoreMoveLookInput(Player1, true);
	SetIgnoreMoveLookInput(Player2, true);

	SetupPlayerLocation(Player1, Player1->GetSeat());
	SetupPlayerLocation(Player2, Player2->GetSeat());

	SetUI(Player1, true);
	SetUI(Player2, true);

	StartRound();

	bGameIsDone = false;
}

void UTurnBasedSession::EndGame()
{
	SetIgnoreMoveLookInput(Player1, false);
	SetIgnoreMoveLookInput(Player2, false);

	SetupPlayerLocation(Player1, Player1InitialTransform);
	SetupPlayerLocation(Player2, Player2InitialTransform);

	SetUI(Player1, false);
	SetUI(Player2, false);

	bGameIsDone = true;

	OnEndGame.Broadcast();
}

void UTurnBasedSession::SetIgnoreMoveLookInput(UGameAgent* Player, bool Value)
{
	APlayerController* Controller = Player->GetHumanController();
	
	if (!Controller) return;

	Controller->SetIgnoreMoveInput(Value);
	Controller->SetIgnoreLookInput(Value);
}

void UTurnBasedSession::SetupPlayerLocation(UGameAgent* Player, FTransform Transform)
{
	if (!Player) return;

	AActor* PlayerActor = Player->GetActor();
	if (!PlayerActor) return;
	
	if (!Transform.IsValid()) return;
	
	if (Player->GetHumanController())
	{
		PlayerActor->SetActorLocation(Transform.GetLocation());
		Player->GetHumanController()->SetControlRotation(Transform.Rotator());
	}
	else
	{
		PlayerActor->SetActorLocation(Transform.GetLocation());
		PlayerActor->SetActorRotation(Transform.Rotator());
	}
}

void UTurnBasedSession::SetUI(UGameAgent* Player, bool bOn)
{
	APlayerController* HumanController = Player->GetHumanController();
	if (!HumanController) return;

	UUserWidget* PlayerUIWidget = Player->GetUI();

	HumanController->bShowMouseCursor = bOn;
	
	if (bOn)
	{
		FInputModeUIOnly InputMode;
		if (bOn) InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		HumanController->SetInputMode(InputMode);

		if (PlayerUIWidget) PlayerUIWidget->AddToViewport();
	}
	else
	{
		FInputModeGameOnly InputMode;
		HumanController->SetInputMode(InputMode);
		
		if (PlayerUIWidget) PlayerUIWidget->RemoveFromParent();
	}
}

bool UTurnBasedSession::StartRound()
{
	if (!Player1 || !Player2) return false;

	if (DrawPile.IsEmpty()) return false;

	ShuffleDrawPile();

	for (int i = 0; i < 5; ++i)
	{
		UBaseCard* RandomCard = DrawCard();
		if (RandomCard) Player1->AddCardToHand(RandomCard);
	}
	for (int i = 0; i < 5; ++i)
	{
		UBaseCard* RandomCard = DrawCard();
		if (RandomCard) Player2->AddCardToHand(RandomCard);
	}
	
	Player1->SetPoints(0);
	Player2->SetPoints(0);

	SetTurnOrder();
	
	return true;
}

void UTurnBasedSession::ShuffleDrawPile()
{
	for (int32 i = DrawPile.Num() - 1; i > 0; --i)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		DrawPile.Swap(i, RandomIndex);
	}
}

UBaseCard* UTurnBasedSession::DrawCard()
{
	if (DrawPile.IsEmpty()) return nullptr;

	int LastIndex = DrawPile.Num() - 1;
	UBaseCard* RandomCard = DrawPile[LastIndex];
	DrawPile.RemoveAt(LastIndex);

	return RandomCard;
}


// turn order
void UTurnBasedSession::SetTurnOrder()
{
	for (auto Turn : TurnOrderData->Turns)
	{
		if (Turn == EPlayerType::Player1) TurnOrder.Add(Player1);
		else TurnOrder.Add(Player2);
	}
}

void UTurnBasedSession::ChoosePlayerTurn()
{
	if (TurnOrder.IsEmpty()) SetTurnOrder();

	CurrentPlayer = TurnOrder.Last();
	TurnOrder.RemoveAt(TurnOrder.Num() - 1);

	
	CurrentPlayer->SetTurn(true);

	for (auto Player : AllPlayers)
	{
		if (Player == CurrentPlayer) continue;
		Player->SetTurn(false);
	}
}


// player actions
void UTurnBasedSession::CurrentPlayerTurnStart()
{
	CurrentPlayer->SetTurnTime(60);
}

void UTurnBasedSession::CurrentPlayerTick(float DeltaTime)
{
	CurrentPlayer->Tick(DeltaTime);
}

void UTurnBasedSession::PlayCard(UBaseCard* InCard, UGameAgent* InOwner)
{
	InCard->ImmediateEffect();

	PlayedCardsStack.Add(FPlayedCard(InCard, InOwner));	//add to bottom of stack

	InOwner->RemoveCardFromHand(InCard);
	
	InOwner->SetPlayedCard(true);
	

	//UI Broadcasts
	OnCardPlayed.Broadcast(InCard, InOwner);
}

void UTurnBasedSession::DisablePlayers()
{
	for (auto Player : AllPlayers)
	{
		Player->SetTurn(false);
	}
}

bool UTurnBasedSession::TryEndRound()
{
	if (Player1->GetCardsInHand().IsEmpty() && Player2->GetCardsInHand().IsEmpty())
	{
		UGameAgent* Winner = ResolvePlayedCardsStack();

		for (auto Player : AllPlayers)
		{
			Player->DisplayWinner(Winner);
		}
		
		return true;
	}
	return false;
}

UGameAgent* UTurnBasedSession::ResolvePlayedCardsStack()
{
	for (int i = PlayedCardsStack.Num()-1; i >= 0; --i)
	{
		UBaseCard* Card = PlayedCardsStack[i].Card;
		UGameAgent* Owner = PlayedCardsStack[i].Owner;
		
		Card->StackEffect();
		Owner->AddPoints(Card->Points);
		
		PlayedCardsStack.RemoveAt(i);
	}

	if (Player1->GetTotalPoints() > Player2->GetTotalPoints()) return Player1;
	return Player2;
}

TArray<UGameAgent*> UTurnBasedSession::GetAllPlayers()
{
	return AllPlayers;
}
