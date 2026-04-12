#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Scoopable.generated.h"

struct FResourceAmount;

UINTERFACE()
class UScoopable : public UInterface
{
	GENERATED_BODY()
};

class COFFEESHOPGAME_API IScoopable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FResourceAmount> Scoop(float AmountToScoop);
};
