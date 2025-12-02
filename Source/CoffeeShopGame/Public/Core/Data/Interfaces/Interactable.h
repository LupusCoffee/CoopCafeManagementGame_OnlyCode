#pragma once

#include "CoreMinimal.h"
#include "Systems/Interaction System/Enums/ActionId.h"
#include "Systems/Interaction System/Structs/InteractionContext.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class COFFEESHOPGAME_API IInteractable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hover(FInteractionContext Context);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Unhover(FInteractionContext Context);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool InteractStarted(EActionId ActionId, FInteractionContext Context);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool InteractOngoing(EActionId ActionId, FInteractionContext Context);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool InteractCompleted(EActionId ActionId, FInteractionContext Context);
};
