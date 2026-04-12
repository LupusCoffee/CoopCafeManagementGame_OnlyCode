// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CoffeeShopGame/Customer/Customer.h"
#include "AIRoamArea.generated.h"

class UBoxComponent;
class UGoapComponent;

USTRUCT()
struct FRoamingAIData
{
	GENERATED_BODY()
	
	UPROPERTY()
	ACustomer* Customer = nullptr;
	
	UPROPERTY()
	float RoamingTime = 0.f;
	
	UPROPERTY()
	bool bShouldBeRemoved = false;
};

UCLASS()
class COFFEESHOPGAME_API AAIRoamArea : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIRoamArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "AI|Action")
	void RequestAIRoam(ACustomer* Agent, float RoamingTime);

	UFUNCTION(Server, Reliable)
	void Server_RequestAIRoam(ACustomer* Agent, float RoamingTime);
	
	
	
private:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Roaming|Delay", meta = (AllowPrivateAccess = "true"))
	bool bUseRandomMoveDelay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Roaming|Delay", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", EditCondition = "!bUseRandomMoveDelay", EditConditionHides))
	float FixedMoveDelaySeconds = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Roaming|Delay", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", EditCondition = "bUseRandomMoveDelay", EditConditionHides))
	float RandomMoveDelayMinSeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Roaming|Delay", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", EditCondition = "bUseRandomMoveDelay", EditConditionHides))
	float RandomMoveDelayMaxSeconds = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Roaming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> RoamingBounds;

	UFUNCTION()
	void HandleCustomerMoveCompleted(ACustomer* FinishedCustomer);

	UFUNCTION()
	void HandleCustomerGoapActionCompleted(const FGameplayTagContainer& EventTags, AActor* Agent);

	UFUNCTION()
	void HandleCustomerGoapActionFailed(const FGameplayTagContainer& EventTags, AActor* Agent);

	void UnbindCustomerMoveCompleted(ACustomer* Customer);
	void UnbindCustomerGoapDelegates(ACustomer* Customer);
	void RemoveRoamingCustomer(ACustomer* Customer);

	UFUNCTION()
	FVector GetPointInZone() const;
	
	UPROPERTY()
	TArray<FRoamingAIData> CustomersRoaming;
};
