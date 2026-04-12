#pragma once


#include "CoreMinimal.h"
#include "Machine.h"
#include "GameFramework/Actor.h"
#include "MachinePart.generated.h"
class UItemPromptComponent;
class UHighlightComponent;


UCLASS()
class COFFEESHOPGAME_API AMachinePart : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AMachinePart();
	
	UFUNCTION()
	virtual bool Init(AMachine* _Owner, FName _ParentSocket);

protected:	
	//Variables --> Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BaseMeshComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHighlightComponent* HighlightComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemPromptComponent* ItemPromptComp = nullptr;

	
	//Variables --> Hidden
	UPROPERTY(BlueprintReadOnly)
	AMachine* OwnerMachine = nullptr;
	
	UPROPERTY()
	FName ParentSocket;
};
