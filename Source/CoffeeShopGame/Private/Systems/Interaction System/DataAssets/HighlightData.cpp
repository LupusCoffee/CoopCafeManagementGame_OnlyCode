#include "Systems/Interaction System/DataAssets/HighlightData.h"

UMaterialInterface* UHighlightData::GetHighlightMaterial()
{
	return HighlightMaterial;
}

FName UHighlightData::GetHighlightMatColorVariableName()
{
	return HighlightMatColorVariableName;
}

FLinearColor UHighlightData::GetColor(EActorType ActorType)
{
	if (const FLinearColor* Found = HighlightColors.Find(ActorType))
	{
		return *Found;
	}
	return DefaultColor; // fallback
}
