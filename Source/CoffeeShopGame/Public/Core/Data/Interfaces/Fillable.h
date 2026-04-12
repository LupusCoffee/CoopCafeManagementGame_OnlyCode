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
	void PourInto(EResourceType ResourceTypeToAdd, float FilledVolume);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Dispense(float DrainedVolume);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void MixContents();
};
