// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOAPSystem/AI/Components/ActivatableComponent.h"
#include "GOAPSystem/AI/Components/AIDebugComponent.h"
#include "GOAPSystem/GOAP/Components/GoapComponent.h"
#include "BaseAICharacter.generated.h"

UCLASS()
class GOAPSYSTEM_API ABaseAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAICharacter();
	
	//---------------------COMPONENTS---------------------//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UActivatableComponent* ActivatableComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGoapComponent* GoapComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIDebugComponent* DebugComp;
	
	
	//---------------------LIFECYCLE---------------------//
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//---------------------POOL INTERFACE---------------------//
	UFUNCTION(BlueprintNativeEvent, Category = "AI Pool")
	void OnPoolSpawned(FVector Location, FRotator Rotation);
	
	UFUNCTION(BlueprintNativeEvent, Category = "AI Pool")
	void OnPoolReturned();
	
protected:
	/** Debug name for AI Manager */
	UPROPERTY(EditDefaultsOnly, Category = "AI Manager")
	FString DebugName;
    
	/** Whether to auto-register with AI Manager */
	UPROPERTY(EditDefaultsOnly, Category = "AI Manager")
	bool bAutoRegister = true;
};
