#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Systems/StatModificationSystem/Components/StatHandlerComponent.h"
#include "StatHandlerCompatibleCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UStatHandlerCompatibleCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UStatHandlerCompatibleCharacterMovementComponent();

	UFUNCTION(BlueprintCallable)
	void BlockMovementInput(bool bInput);

	UFUNCTION(BlueprintCallable)
	void BlockJumpInput(bool bInput);

	UFUNCTION(BlueprintCallable)
	void BlockMovementAndJumpInput(bool bInput);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsBlockingMovementInput();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsBlockingJumpInput();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	bool bBlockMovementInput = false;

	UPROPERTY(Replicated)
	bool bBlockJumpInput = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatHandlerComponent* StatHandler = nullptr;

	UFUNCTION()
	void OnStatModified();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
};
