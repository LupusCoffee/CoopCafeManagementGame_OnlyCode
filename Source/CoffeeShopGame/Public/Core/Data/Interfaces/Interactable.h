#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Enums/ActionId.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Structs/PlayerContext.h"
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
	//Local Hover
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Local_StartHover(FPlayerContext Context);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Local_TickHover(FPlayerContext Context, float DeltaTime);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Local_EndHover(FPlayerContext Context);
	
	
	//Local Interaction
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Local_StartInteraction(EActionId ActionId, FPlayerContext Context);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Local_TickInteraction(EActionId ActionId, FPlayerContext Context, float DeltaTime);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Local_EndInteraction(EActionId ActionId, FPlayerContext Context);
	
	
	//Server Interaction
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Server_StartInteraction(EActionId ActionId, FPlayerContext Context);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Server_TickInteraction(EActionId ActionId, FPlayerContext Context, float DeltaTime);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Server_EndInteraction(EActionId ActionId, FPlayerContext Context);
	
	
	//AI Interaction
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AIInteract(APawn* Agent);
};