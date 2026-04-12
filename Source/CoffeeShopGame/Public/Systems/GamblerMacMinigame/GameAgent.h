#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameAgent.generated.h"
class AGamblerMacAiController;
class UTurnBasedSession;
class UCardGameUI;
class UBaseCard;

UCLASS(BlueprintType)
class COFFEESHOPGAME_API UGameAgent : public UObject
{
	GENERATED_BODY()

public:
	//Setup
	UFUNCTION()
	void Setup(
		AActor* InActor, AController* InController, FTransform InPlayerSeat,
		UTurnBasedSession* InSession, TSubclassOf<UCardGameUI> InCardWidget, FName InName);


	//Tick
	UFUNCTION()
	void Tick(float DeltaTime);

	
	//Setters
	// function
	UFUNCTION()
	void SetTurn(bool bMyTurn);

	// points
	UFUNCTION()
	void SetPoints(int Value);

	UFUNCTION()
	void AddPoints(int Value);

	UFUNCTION()
	void RemovePoints(int Value);

	// cards
	UFUNCTION()
	void AddCardToHand(UBaseCard* Card);

	UFUNCTION()
	void RemoveCardFromHand(UBaseCard* Card);

	// context
	UFUNCTION()
	void SetTurnTime(float Value);
	
	UFUNCTION()
	void SetPlayedCard(bool Value);

	// other ui
	UFUNCTION(BlueprintCallable)
	void DisplayWinner(UGameAgent* Winner);
	

	//Getters
	// general
	UFUNCTION()
	AActor* GetActor();

	UFUNCTION()
	AController* GetAIController();

	UFUNCTION(BlueprintCallable)
	APlayerController* GetHumanController();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAI();
	
	UFUNCTION()
	FTransform GetSeat();

	// ui
	UFUNCTION()
	UUserWidget* GetUI();

	// logic
	UFUNCTION()
	FName GetAgentName();

	UFUNCTION()
	int GetTotalPoints();

	UFUNCTION()
	TArray<UBaseCard*> GetCardsInHand() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTurnTime();
	
	UFUNCTION()
	bool GetPlayedCard();

	
private:
	// general actors, controllers, seat, etc.
	UPROPERTY()
	AActor* Actor = nullptr;
	
	UPROPERTY()
	AGamblerMacAiController* AiController = nullptr;

	UPROPERTY()
	APlayerController* HumanController = nullptr;

	UPROPERTY()
	bool bIsAI = false;

	UPROPERTY()
	FTransform PlayerSeat;

	// ui
	UPROPERTY()
	UCardGameUI* CardGameUIWidget;


	// game logic
	UPROPERTY()
	FName AgentName = "Hinata the Null";
	
	UPROPERTY(VisibleAnywhere)
	int TotalPoints = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<UBaseCard*> CardsInHand;


	// context
	UPROPERTY()
	float TurnTimeLeft = 0;

	UPROPERTY()
	bool bPlayedCard = false;
};
