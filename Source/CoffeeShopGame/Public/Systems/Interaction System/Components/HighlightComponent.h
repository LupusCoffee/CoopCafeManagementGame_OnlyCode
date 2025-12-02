//Note: move to "core/components" if used by more than interaction system

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Data/Enums/ActorType.h"
#include "HighlightComponent.generated.h"
class UHighlightRegistrySubsystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHighlightComponent();

	UPROPERTY(EditAnywhere, Category = "Highlight")
	bool bCanHighlight = true;

	UFUNCTION(BlueprintCallable, Category = "Highlight")
	void EnableHighlight();

	UFUNCTION(BlueprintCallable, Category = "Highlight")
	void DisableHighlight();

protected:
	virtual void BeginPlay() override;

	//Variables --> Editable
	UPROPERTY(EditAnywhere, Category = "Highlight")
	EActorType ActorType;
	
	//Variables --> Hidden
	UPROPERTY()
	UHighlightRegistrySubsystem* HighlightRegistry = nullptr;
	
	UPROPERTY()
	UStaticMeshComponent* HighlightedMesh = nullptr;
};
