#pragma once


#include "CoreMinimal.h"
#include "BaseCard.h"
#include "GameAgent.h"
#include "Blueprint/UserWidget.h"
#include "CardGameUI.generated.h"
class UCardUI;
class UTurnBasedSession;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateCards, const TArray<UBaseCard*>&, Cards, bool, bCurrentlyMyTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateTurnStatus, bool, bMyTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDisplayWinner, UGameAgent*, Winner, UGameAgent*, Me);


UCLASS()
class COFFEESHOPGAME_API UCardGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Setup(UTurnBasedSession* InSession, UGameAgent* InPlayer);
	
	UFUNCTION()
	void UpdateCards();

	UFUNCTION()
	void SetTurn(bool Value);

	UFUNCTION()
	void DisplayWinner(UGameAgent* Winner, UGameAgent* Me);
	

	UPROPERTY(BlueprintAssignable)
	FOnUpdateCards OnUpdateCards;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateTurnStatus OnUpdateTurnStatus;

	UPROPERTY(BlueprintAssignable)
	FOnDisplayWinner OnDisplayWinner;
	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTurnBasedSession* GetSession();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGameAgent* GetPlayer();

	
private:
	UPROPERTY()
	bool bMyTurn = false;
	
	UPROPERTY()
	UTurnBasedSession* Session;

	UPROPERTY()
	UGameAgent* Player;
};
