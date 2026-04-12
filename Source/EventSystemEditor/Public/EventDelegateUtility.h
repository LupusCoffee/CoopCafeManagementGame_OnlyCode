#pragma once

#include "EventCollection/DetailPanelDataObject.h"
#include "EventDelegateUtility.generated.h"

struct FDataTag;

UCLASS()
class EVENTSYSTEMEDITOR_API UEventDelegateUtility : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void CreateEventDispatcher(UBlueprint* TargetBlueprint, FString DesiredName, TArray<FDataTag> Params);
	static void DeleteEventDispatcher(UBlueprint* TargetBlueprint, FName EventName);
	static void UpdateDispatcherParameters(UBlueprint* TargetBlueprint, FString EventName, TArray<FDataTag> Params, UDetailPanelDataObject* obj);
};