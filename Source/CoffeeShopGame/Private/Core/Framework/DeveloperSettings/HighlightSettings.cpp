#include "Core/Framework/DeveloperSettings/HighlightSettings.h"

UMaterialInterface* UHighlightSettings::GetHighlightMaterial()
{
	return HighlightMaterial.LoadSynchronous();
}

const FName UHighlightSettings::GetHighlightMatColorVariableName()
{
	return HighlightMatColorVariableName;
}

const FLinearColor UHighlightSettings::GetColor(EActorType ActorType)
{
	if (const FLinearColor* Found = HighlightColors.Find(ActorType))
	{
		return *Found;
	}
	return DefaultColor; // fallback
}