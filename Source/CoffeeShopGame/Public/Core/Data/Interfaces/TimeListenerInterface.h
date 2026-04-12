// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TimeListenerInterface.generated.h"

struct FEventData;

UINTERFACE()
class UTimeListenerInterface : public UInterface
{
	GENERATED_BODY()
};

class COFFEESHOPGAME_API ITimeListenerInterface
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//TArray<FEventData> GetEventData();

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//void OnEventTriggered(FString InEvent) {};
};
