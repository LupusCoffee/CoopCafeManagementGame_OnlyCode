#pragma once

#include "CoreMinimal.h"
#include "BaseCard.h"
#include "CardGroupData.h"
#include "GameAgent.h"
#include "TurnOrder.h"
#include "UObject/Object.h"
#include "TurnBasedSession.generated.h"
class UTurnOrder;
class UCardGameUI;
class APlayerCharacter;
class UActionStack;

USTRUCT(BlueprintType)
struct FPlayedCard
{
	GENERATED_BODY()

	FPlayedCard(): Card(nullptr), Owner(nullptr){}

	FPlayedCard(UBaseCard* InCard, UGameAgent* InOwner): Card(InCard), Owner(InOwner){}

	UPROPERTY()
	TObjectPtr<UBaseCard> Card = nullptr;

	UPROPERTY()
	TObjectPtr<UGameAgent> Owner = nullptr;
};

//Should also be in a child session
//Delegate Declerations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCardPlayed, UBaseCard*, PlayedCard, UGameAgent*, CardOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndGame);

UCLASS(BlueprintType, Blueprintable)
class COFFEESHOPGAME_API UTurnBasedSession : public UObject
{
	GENERATED_BODY()

	
public:
	//Setup
	UFUNCTION()
	void SetupSession(
		AActor* InPlayer1, AController* InController1,
		AActor* InPlayer2, AController* InController2,
		FTransform InPlayerSeat1, FTransform InPlayerSeat2,
		TSubclassOf<UCardGameUI> InCardWidget, UCardGroupData* InDrawPile,
		UTurnOrder* InTurnOrderData, float InTurnTime); //todo: make this more generic

	
	//Tick
	void Tick(float DeltaTime);

	
	//Getters
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UActionStack* GetActionStack();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool TryEndRound();

	UFUNCTION()
	UGameAgent* ResolvePlayedCardsStack();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UGameAgent*> GetAllPlayers();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGameAgent* GetCurrentPlayer();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTurnTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetGameIsDone();

	
	//todo: make it so this is something a child class has instead
	//Action Functions - Setup
	UFUNCTION()
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void EndGame();

	
	//Action Functions - Game Logic
	UFUNCTION()
	void ChoosePlayerTurn();

	UFUNCTION()
	void CurrentPlayerTurnStart();

	UFUNCTION()
	void CurrentPlayerTick(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void PlayCard(UBaseCard* InCard, UGameAgent* InOwner);

	UFUNCTION(BlueprintCallable)
	void DisablePlayers();

	
	//Delegates - Update Game Area, UI, etc.
	UPROPERTY(BlueprintAssignable)
	FOnCardPlayed OnCardPlayed;

	UPROPERTY(BlueprintAssignable)
	FOnEndGame OnEndGame;
	
	
private:
	//Methods
	UFUNCTION()
	void SetIgnoreMoveLookInput(UGameAgent* Player, bool Value);
	
	UFUNCTION()
	void SetupPlayerLocation(UGameAgent* Player, FTransform Transform);

	UFUNCTION()
	void SetUI(UGameAgent* Player, bool bOn);
	
	UFUNCTION()
	bool StartRound();

	UFUNCTION()
	void ShuffleDrawPile();

	UFUNCTION()
	UBaseCard* DrawCard();

	UFUNCTION()
	void SetTurnOrder();
	
	
	//Variables - Action Stack
	UPROPERTY()
	UActionStack* ActionStack;

	UPROPERTY()
	UTurnOrder* TurnOrderData;

	
	// all of these should be in a child class instead --> also, maybe i should divide all of this into a
	// setup manager and a game master class that the child of this turn based session class would use?
	//Variables - Setup
	UPROPERTY()
	FTransform Player1InitialTransform;

	UPROPERTY()
	FTransform Player2InitialTransform;

	UPROPERTY()
	float TurnTime = 15;
	
	//Variables - Dynamic
	UPROPERTY()
	TArray<UGameAgent*> AllPlayers;
	
	UPROPERTY(VisibleAnywhere)
	UGameAgent* Player1;

	UPROPERTY(VisibleAnywhere)
	UGameAgent* Player2;

	UPROPERTY(VisibleAnywhere)
	UGameAgent* CurrentPlayer;

	UPROPERTY(VisibleAnywhere)
	TArray<FPlayedCard> PlayedCardsStack;

	UPROPERTY(VisibleAnywhere)
	TArray<UBaseCard*> DrawPile;
	
	UPROPERTY()
	TArray<UGameAgent*> TurnOrder;

	UPROPERTY()
	bool bGameIsDone = false;
};