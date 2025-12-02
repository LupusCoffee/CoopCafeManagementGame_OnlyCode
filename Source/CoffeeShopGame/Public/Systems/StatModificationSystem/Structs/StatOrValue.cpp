#include "StatOrValue.h"
#include "Systems/StatModificationSystem/Components/StatHandlerComponent.h"

float FStatOrValue::GetValue(UStatHandlerComponent* StatSystem) const	//slightly scuffed
{
	if (UsesStat())
	{
		float Value = 0;
		if (StatSystem->TryGetModifiedPropertyStat(StatTag, Value)) return Value;
	}
	
	return LiteralValue;
}
