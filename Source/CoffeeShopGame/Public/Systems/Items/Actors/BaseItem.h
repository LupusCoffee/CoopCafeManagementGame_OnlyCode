#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UHighlightComponent;

UCLASS()
class COFFEESHOPGAME_API ABaseItem : public AActor, public IInteractable
{
	GENERATED_BODY()

	
public:
	ABaseItem();

	//Methods --> Getters
	UStaticMeshComponent* GetMeshComponent() { return MeshComp; }


protected:

	//Variables --> Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHighlightComponent* HighlightComponent = nullptr;

	//Methods --> Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
};
