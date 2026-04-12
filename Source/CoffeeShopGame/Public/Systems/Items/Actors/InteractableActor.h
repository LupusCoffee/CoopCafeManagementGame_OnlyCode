#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

class UItemPromptComponent;
class UHighlightComponent;

UCLASS()
class COFFEESHOPGAME_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	AInteractableActor();

	//Methods --> Getters
	UStaticMeshComponent* GetMeshComponent() { return MeshComp; }

protected:
	//Variables --> Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHighlightComponent* HighlightComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemPromptComponent* ItemPromptComp = nullptr;
};
