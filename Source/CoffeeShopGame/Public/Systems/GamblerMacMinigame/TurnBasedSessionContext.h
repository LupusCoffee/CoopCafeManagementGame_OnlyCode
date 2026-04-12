#pragma once

#include "CoreMinimal.h"
#include "Systems/ActionStack/ActionStackContext.h"
#include "TurnBasedSessionContext.generated.h"
class TurnBasedSession;

UCLASS(Blueprintable)
class COFFEESHOPGAME_API UTurnBasedSessionContext : public UActionStackContext
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Init(UTurnBasedSession* InSession)
	{
		Session = InSession;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTurnBasedSession> Session = nullptr;
};
