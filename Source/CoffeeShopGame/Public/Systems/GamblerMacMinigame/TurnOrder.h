#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TurnOrder.generated.h"

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	Player1			UMETA(DisplayName = "Player1"),
	Player2			UMETA(DisplayName = "Player2")
};

UCLASS()
class COFFEESHOPGAME_API UTurnOrder : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<EPlayerType> Turns;
};
