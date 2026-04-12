#include "EventTagSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EventTagSettings)

UTagList::UTagList(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// No config filename, needs to be set at creation time
}

UEventTagSettings::UEventTagSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConfigFileName = GetDefaultConfigFilename();
}