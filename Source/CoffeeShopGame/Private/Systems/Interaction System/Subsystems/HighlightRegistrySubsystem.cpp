#include "Systems/Interaction System/Subsystems/HighlightRegistrySubsystem.h"
#include "Core/Framework/DeveloperSettings/HighlightSettings.h"
#include "Core/Framework/Subsystems/PostProcessSubsystem.h"

//Overrides
void UHighlightRegistrySubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	HighlightSettings = GetMutableDefault<UHighlightSettings>();

	bool UnboundPostProcessVolumeSetupSuccessful = UnboundPostProcessVolumeHighlightEffectSetup();
	if (!UnboundPostProcessVolumeSetupSuccessful) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red,
		"UnboundPostProcessVolume HighlightEffect Setup... INCOMPLETE DESU YO!!!!");
}

void UHighlightRegistrySubsystem::Deinitialize()
{
	HighlightSettings = nullptr;
	UnboundPostProcessVolume = nullptr;
	PostProcessVolumeHighlightMaterial = nullptr;
	CachedHighlightMaterials.Empty();
	
	Super::Deinitialize();
}

//Methods
bool UHighlightRegistrySubsystem::UnboundPostProcessVolumeHighlightEffectSetup()
{
	UWorld* World = GetWorld();
	if (!World) return false;
	
	if (auto* PostProcessSubsystem = World->GetSubsystem<UPostProcessSubsystem>()) //dependency: maybe refactor?
		UnboundPostProcessVolume = PostProcessSubsystem->GetUnboundPostProcessVolume();
	else return false;

	if (UnboundPostProcessVolume)
	{
		PostProcessVolumeHighlightMaterial = UMaterialInstanceDynamic::Create(
			HighlightSettings->GetHighlightMaterial(), this, TEXT("MID_Highlight"));
		
		UnboundPostProcessVolume->Settings.AddBlendable(PostProcessVolumeHighlightMaterial, 1.0f);
	}
	else return false;
	
	return true;
}

void UHighlightRegistrySubsystem::UpdateHighlightMaterial(EActorType ActorType)
{
	const FName HighlightVariableName = HighlightSettings->GetHighlightMatColorVariableName();
	const FLinearColor HighlightColor = HighlightSettings->GetColor(ActorType);
	
	if (PostProcessVolumeHighlightMaterial)
	{
		PostProcessVolumeHighlightMaterial->SetVectorParameterValue(HighlightVariableName, HighlightColor);
		CurrentHighlightColor = HighlightColor;
	}
}

FLinearColor UHighlightRegistrySubsystem::GetCurrentHighlightColor()
{
	return CurrentHighlightColor;
}


//creates and caches material of ActorType if never mentioned before --> kinda useless now lol
UMaterialInstanceDynamic* UHighlightRegistrySubsystem::GetHighlightMaterial(EActorType ActorType)
{
	if (!HighlightSettings) return nullptr;

	if (UMaterialInstanceDynamic** CreatedDynMat = CachedHighlightMaterials.Find(ActorType))
	{
		return *CreatedDynMat;
	}

	UMaterialInterface* HighlightMat = HighlightSettings->GetHighlightMaterial();
	if (!HighlightMat) return nullptr;
	
	UMaterialInstanceDynamic* NewHighlightMaterial = UMaterialInstanceDynamic::Create(HighlightMat, this);
	if (!NewHighlightMaterial) return nullptr;
	
	NewHighlightMaterial->SetVectorParameterValue(HighlightSettings->GetHighlightMatColorVariableName(), HighlightSettings->GetColor(ActorType));

	CachedHighlightMaterials.Add(ActorType, NewHighlightMaterial);
	return NewHighlightMaterial;
}
