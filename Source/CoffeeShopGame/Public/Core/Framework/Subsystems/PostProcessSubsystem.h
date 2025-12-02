#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PostProcessSubsystem.generated.h"

UCLASS()
class COFFEESHOPGAME_API UPostProcessSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//Methods --> Getters
	UFUNCTION(BlueprintCallable)
	APostProcessVolume* GetUnboundPostProcessVolume();
	

private:
	//Overrides
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	//Variables --> Hidden
	UPROPERTY()
	APostProcessVolume* UnboundPostProcessVolume = nullptr;

	//Methods --> Creators
	UFUNCTION()
	APostProcessVolume* CreateUnboundPostProcessVolume();
};
