#pragma once

#include "CoreMinimal.h"
#include "EventSystemStructs.h"
#include "EventMulticastDelegate.generated.h"

USTRUCT()
struct FEventMulticastDelegate
{
	GENERATED_BODY()

	FName EventName;

	UPROPERTY()
	UObject* TargetObject;


	void DeleteEventDispatcher();
};