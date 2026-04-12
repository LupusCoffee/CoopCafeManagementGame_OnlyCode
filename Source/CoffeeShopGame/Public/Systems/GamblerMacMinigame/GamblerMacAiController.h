#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "GamblerMacAiController.generated.h"

class UTurnBasedSession;
class UGameAgent;

UCLASS()
class COFFEESHOPGAME_API AGamblerMacAiController : public AController
{
	GENERATED_BODY()

public:
	AGamblerMacAiController();
	virtual void BeginPlay() override;

	UFUNCTION()
	void Setup(UTurnBasedSession* InSession, UGameAgent* InAgent);

	UFUNCTION()
	void SetTurn(bool bInMyTurn);

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	bool bMyTurn = false;

	UPROPERTY()
	UTurnBasedSession* Session = nullptr;

	UPROPERTY()
	UGameAgent* Agent = nullptr;

	UPROPERTY()
	float MaxTime = 6;

	UPROPERTY()
	float MinTime = 3;

	UPROPERTY()
	float WaitTimeBeforeChoosingCard = 3;

	UPROPERTY()
	float CurrentTime = 0;
};
