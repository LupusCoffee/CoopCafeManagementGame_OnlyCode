#include "EventTagBlueprintLibrary.h"
#include "EventSystemStructs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EventTagBlueprintLibrary)

UEventTagBlueprintLibrary::UEventTagBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
bool UEventTagBlueprintLibrary::EqualEqual_EventTag(FEventTag A, FEventTag B)
{
	return A == B;
}

bool UEventTagBlueprintLibrary::NotEqual_EventTag(FEventTag A, FEventTag B)
{
	return A != B;
}