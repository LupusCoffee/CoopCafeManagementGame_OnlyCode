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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatHandlerComponent* StatHandler = nullptr;

	UFUNCTION()
	void OnStatModified();
};
