#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/EngineTypes.h"
#include "ServiceBootstrapperSettings.generated.h"

class AActor;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Service Bootstrapper"))
class COFFEESHOPGAME_API UServiceBootstrapperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UServiceBootstrapperSettings();

	// Actor classes spawned automatically for each game world.
	UPROPERTY(Config, EditAnywhere, Category="Bootstrap")
	TArray<TSoftClassPtr<AActor>> ActorClassesToSpawn;

	// If true, services spawn only in authority worlds (server/listen-host).
	UPROPERTY(Config, EditAnywhere, Category="Bootstrap")
	bool bSpawnOnlyOnAuthority = true;

	UPROPERTY(Config, EditAnywhere, Category="Bootstrap")
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandling =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	static const UServiceBootstrapperSettings* Get()
	{
		return GetDefault<UServiceBootstrapperSettings>();
	}
};

