#include "Core/Framework/DeveloperSettings/LookTraceSettings.h"

float ULookTraceSettings::GetTraceLength()
{
	return TraceLength;
}

float ULookTraceSettings::GetTraceRadius()
{
	return TraceRadius;
}

TEnumAsByte<ECollisionChannel> ULookTraceSettings::GetTraceChannel()
{
	return TraceChannel;
}

bool ULookTraceSettings::DoesDrawDebug()
{
	return bDrawTraceDebug;
}
