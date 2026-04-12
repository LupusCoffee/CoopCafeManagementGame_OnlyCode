#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Subsystems/WorldSubsystem.h"
#include "TurnBasedManager.generated.h"
class UCardGameUI;
class UCardGroupData;
class UTurnBasedSession;
class UTurnOrder;

UCLASS()
class COFFEESHOPGAME_API UTurnBasedManager : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//Setup
	virtual bool IsTickable() const override
	{
		UWorld* World = GetWorld();
		return !IsTemplate() && World && World->IsGameWorld() && !World->IsPaused();
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UGamblerMacMinigameManager, STATGROUP_Tickables);
	}
	
	//Tick
	virtual void Tick(float DeltaTime) override;

	//Sessions
	UFUNCTION(BlueprintCallable)
	UTurnBasedSession* StartGame(
		AActor* InPlayer1, AController* InController1,
		AActor* InPlayer2, AController* InController2,
		FTransform InPlayerSeat1, FTransform InPlayerSeat2,
		TSubclassOf<UCardGameUI> InCardWidget, UCardGroupData* InDrawPile,
		UTurnOrder* InTurnOrderData, float InTurnTime);

private:
	UPROPERTY()
	TArray<UTurnBasedSession*> GameSessions;
};
