#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Interfaces/StatHandleable.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"
class UHolderComponent;
class UInteractionComponent;
class UStatHandlerCompatibleCharacterMovementComponent;

UCLASS()
class COFFEESHOPGAME_API APlayerCharacter : public ACharacter, public IStatHandleable
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	//Methods
	UFUNCTION(BlueprintCallable)
	UInteractionComponent* GetInteractionComponent();

	UFUNCTION(BlueprintCallable)
	UHolderComponent* GetHolderComponent();
	
	UFUNCTION(BlueprintCallable)
	UStatHandlerCompatibleCharacterMovementComponent* GetStatHandleableCharMoveComp();

protected:
	//Variables, Visible --> Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInteractionComponent* InteractionComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHolderComponent* HolderComponent = nullptr;

	UPROPERTY()
	UStatHandlerCompatibleCharacterMovementComponent* StatHandlableMovementComponent = nullptr;
};
