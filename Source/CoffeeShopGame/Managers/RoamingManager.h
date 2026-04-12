// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOAPSystem/AI/Interface/Interractable.h"
#include "RoamingManager.generated.h"

class ACustomer;
class AAIRoamArea;

UCLASS()
class COFFEESHOPGAME_API ARoamingManager : public AActor, public IGOAPInteractable
{
public:
	virtual void Interact_Implementation(AActor* Agent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoamingManager();
	
	UFUNCTION(BlueprintCallable, Category= "Management")
	void AddRoamArea(AAIRoamArea* RoamArea);
	
	UFUNCTION(BlueprintCallable, Category= "AI|Action")
	void RequestAIRoam(ACustomer* Agent, float RoamingTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	UFUNCTION(Server, Reliable)
	void Server_RequestAIRoam(ACustomer* Agent, float RoamingTime);

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AAIRoamArea>> RoamAreas;
};
