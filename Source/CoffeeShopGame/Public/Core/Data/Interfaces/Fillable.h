#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Fillable.generated.h"

UINTERFACE()
class UFillable : public UInterface
{
	GENERATED_BODY()
};

class COFFEESHOPGAME_API IFillable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Recieve(EResourceType ResourceTypeToAdd, float FilledVolume);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Dispense(float DrainedVolume);
};
