#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LookTraceSettings.generated.h"


UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Look Trace"))
class COFFEESHOPGAME_API ULookTraceSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//Methods
	UFUNCTION(BlueprintPure, Category="LookTrace")
	float GetTraceLength();

	UFUNCTION(BlueprintPure, Category="LookTrace")
	float GetTraceRadius();

	UFUNCTION(BlueprintPure, Category="LookTrace")
	TEnumAsByte<ECollisionChannel> GetTraceChannel();

	UFUNCTION(BlueprintPure, Category="LookTrace")
	bool DoesDrawDebug();

protected:
	//Variables --> Edit
	UPROPERTY(EditAnywhere, Config, Category="Interaction")
	float TraceLength = 500.0f;

	UPROPERTY(EditAnywhere, Config, Category="Interaction")
	float TraceRadius = 12.0f;

	UPROPERTY(EditAnywhere, Config, Category="Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, Config, Category="Interaction")
	bool bDrawTraceDebug = false;
};
