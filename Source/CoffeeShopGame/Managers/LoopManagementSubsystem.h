#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopGame/Customer/Queue/Queue.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "Subsystems/WorldSubsystem.h"
#include "LoopManagementSubsystem.generated.h"

class ATicketManager;
/**
 * 
 */
UCLASS()
class COFFEESHOPGAME_API ULoopManagementSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:		
	UFUNCTION(BlueprintCallable, Category = "Loop Management")
	void SetTicketManagerReference(ATicketManager* TicketManager);
	
	UFUNCTION(BlueprintCallable, Category = "Loop Management")
	ATicketManager* GetTicketManagerReference() const;
	
	UFUNCTION(BlueprintCallable, Category = "Loop Management")
	void SetQueueReference(AQueue* _queue);
	
	UFUNCTION(BlueprintCallable, Category = "Loop Management")
	AQueue* GetQueueReference() const;
	
	UFUNCTION(BlueprintCallable, Category = "Loop Management|Bell")
	void SetBell(AActor* bellActor);
	
	UFUNCTION(BlueprintCallable, Category = "Loop Management|Bell")
	AActor* GetBellActor() const;

private:
	UPROPERTY()
	TObjectPtr<ATicketManager> TicketManagerReference;
	
	UPROPERTY()
	TObjectPtr<AQueue> Queue;
	
	UPROPERTY()
	TObjectPtr<AActor> Bell;
};
