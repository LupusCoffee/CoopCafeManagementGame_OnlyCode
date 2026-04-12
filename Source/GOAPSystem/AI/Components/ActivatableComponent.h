// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "ActivatableComponent.generated.h"


UCLASS(ClassGroup=(Customer), meta=(BlueprintSpawnableComponent))
class GOAPSYSTEM_API UActivatableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActivatableComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	
	// Main interface
	UFUNCTION(BlueprintCallable, Category = "Activatable")
	void ActivateActor();
	
	UFUNCTION(BlueprintCallable, Category = "Activatable")
	void DeactivateActor();
	
	UFUNCTION(BlueprintCallable, Category = "Activatable")
	bool IsActiveActor() const { return bIsActiveActor; }

	// Pool management helpers
	UFUNCTION(BlueprintCallable, Category = "Activatable")
	void ActivateAtLocation(FVector Location, FRotator Rotation);
	
	UFUNCTION(BlueprintCallable, Category = "Activatable")
	void DeactivateAndMoveToHiddenLocation(FVector HiddenLocation);
	
	UFUNCTION(BlueprintCallable, Category = "Activatable")
	void SetHiddenLocation(FVector Location);
	
	UFUNCTION(BlueprintPure, Category = "Activatable")
	FVector GetHiddenLocation() const { return HiddenLocation; }

	// Events for Blueprint/C++ to hook into
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivatedSignature);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeactivatedSignature);

	UPROPERTY(BlueprintAssignable, Category = "Activatable")
	FOnActivatedSignature OnActivated;

	UPROPERTY(BlueprintAssignable, Category = "Activatable")
	FOnDeactivatedSignature OnDeactivated;

	// Server-authoritative RPCs
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateActor();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DeactivateActor();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateAtLocation(FVector Location, FRotator Rotation);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DeactivateAndMoveToHiddenLocation(FVector InHiddenLocation);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnActivationStateChanged();

	UFUNCTION()
	void OnRep_IsActiveActor();
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_IsActiveActor, EditAnywhere, Category = "Activatable")
	bool bIsActiveActor = false;
	
	// Configuration
	UPROPERTY(EditAnywhere, Category = "Activatable")
	bool bDisableCollisionWhenInactive = true;

	UPROPERTY(EditAnywhere, Category = "Activatable")
	bool bDisableTickWhenInactive = true;

	UPROPERTY(EditAnywhere, Category = "Activatable")
	bool bHideWhenInactive = true;

	UPROPERTY(EditAnywhere, Category = "Activatable")
	bool bDisableAIWhenInactive = true;
	
	//Only avalible for actors that make use of CharacterMovementComponent
	UPROPERTY(EditAnywhere, Category = "Activatable")
	bool bShouldBeAffectedByGravity = true;
	
	UPROPERTY()
	FVector HiddenLocation = FVector(0, 0, -10000);
	
	float baseGravityScale;
};
