#pragma once


#include "CoreMinimal.h"
#include "Machine.h"
#include "GameFramework/Actor.h"
#include "MachinePart.generated.h"
class UHighlightComponent;


UCLASS()
class COFFEESHOPGAME_API AMachinePart : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AMachinePart();
	virtual void Tick(float DeltaTime) override;

	//Methods
	UFUNCTION()
	virtual bool Init(AMachine* _Owner, FName _ParentSocket);

protected:
	virtual void BeginPlay() override;

	
	//Variables --> Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHighlightComponent* HighlightComponent = nullptr;

	
	//Variables --> Hidden
	UPROPERTY()
	AMachine* OwnerMachine = nullptr;
	
	UPROPERTY()
	FName ParentSocket;


	//Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;
};
