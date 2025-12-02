#include "StatOrValueWithModificationType.h"
#include "Systems/StatModificationSystem/Components/StatHandlerComponent.h"

float FStatOrValueWithModificationType::GetValue(UStatHandlerComponent* StatSystem) const	//slightly scuffed
{
	if (UsesStat())
	{
		float Value = 0;
		if (StatSystem->TryGetModifiedPropertyStat(StatTag, Value)) return Value;
	}
	
	return LiteralValue;
}
