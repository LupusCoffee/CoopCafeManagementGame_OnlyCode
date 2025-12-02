#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Enums/ActorType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HighlightRegistrySubsystem.generated.h"
class UHighlightSettings;

//todo: StencilIDs should be used for highlighting multiple objects with different colors --> once the time comes, not now

UCLASS()
class COFFEESHOPGAME_API UHighlightRegistrySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()


public:
	//Methods
	UFUNCTION(BlueprintCallable)
	void UpdateHighlightMaterial(EActorType ActorType);

	UFUNCTION(BlueprintCallable)
	FLinearColor GetCurrentHighlightColor();

private:
	//Overrides
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
	//Variables --> Hidden
	UPROPERTY()
	UHighlightSettings* HighlightSettings = nullptr;

	UPROPERTY()
	APostProcessVolume* UnboundPostProcessVolume = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* PostProcessVolumeHighlightMaterial = nullptr;

	UPROPERTY()
	TMap<EActorType, UMaterialInstanceDynamic*> CachedHighlightMaterials;

	UPROPERTY()
	FLinearColor CurrentHighlightColor = FLinearColor::White;

	//Methods --> Setup
	UFUNCTION()
	bool UnboundPostProcessVolumeHighlightEffectSetup();

	//Methods --> Misc
	UFUNCTION()
	UMaterialInstanceDynamic* GetHighlightMaterial(EActorType ActorType);
};
