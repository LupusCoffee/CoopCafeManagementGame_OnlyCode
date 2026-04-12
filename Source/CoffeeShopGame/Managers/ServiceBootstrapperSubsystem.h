#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ServiceBootstrapperSubsystem.generated.h"

class AActor;

UCLASS()
class COFFEESHOPGAME_API UServiceBootstrapperSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Service Bootstrapper")
	void BootstrapServices();

	UFUNCTION(BlueprintPure, Category="Service Bootstrapper")
	bool HasBootstrapped() const { return bHasBootstrapped; }

private:
	bool bHasBootstrapped = false;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> SpawnedServiceActors;
};

