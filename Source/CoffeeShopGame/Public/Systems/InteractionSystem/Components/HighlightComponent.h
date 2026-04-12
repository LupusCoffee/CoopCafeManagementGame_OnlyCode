#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Data/Enums/ActorType.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "HighlightComponent.generated.h"
class UHighlightRegistrySubsystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UHighlightComponent : public UActorComponent, public IInteractable
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
	
	//Methods --> Hover
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
};
