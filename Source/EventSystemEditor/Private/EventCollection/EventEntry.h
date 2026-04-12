#pragma once

#include "EventSystemStructs.h"

// Placeholder struct for the list view
// By directly referencing FEventData in list view, it seems to delete the event data when TSharedPtr is destroyed
struct FEventEntry
{
	FEventData* EventData;

	FEventEntry();

	FEventEntry(FEventData* InEventData)
	{
		EventData = InEventData;
	}
};