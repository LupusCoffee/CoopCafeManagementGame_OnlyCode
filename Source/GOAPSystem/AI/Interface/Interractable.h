#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interractable.generated.h"

UINTERFACE(Blueprintable)
class UGOAPInteractable : public UInterface
{
	GENERATED_BODY()
};

class GOAPSYSTEM_API IGOAPInteractable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(AActor* Agent);
};

