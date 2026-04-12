#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EventTagBlueprintLibrary.generated.h"

struct FEventTag;

UCLASS(meta = (ScriptName = "EventTagLibrary"), MinimalAPI)
class UEventTagBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Returns true if the values are equal (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (EventTag)", CompactNodeTitle = "==", BlueprintThreadSafe), Category = "EventTag")
	static bool EqualEqual_EventTag(FEventTag A, FEventTag B);

	/** Returns true if the values are not equal (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Not Equal (EventTag)", CompactNodeTitle = "!=", BlueprintThreadSafe), Category = "EventTag")
	static bool NotEqual_EventTag(FEventTag A, FEventTag B);
};