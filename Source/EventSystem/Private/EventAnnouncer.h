#pragma once

#include "CoreMinimal.h"
#include "EventAnnouncer.generated.h"

struct FDataContainer;

UCLASS()
class EVENTSYSTEM_API AEventAnnouncer : public AActor
{
	GENERATED_BODY()

public:
	void InvokeEvent(FName Event, FDataContainer DataContainer);

private:
};