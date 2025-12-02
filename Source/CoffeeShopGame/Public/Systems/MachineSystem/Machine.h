
#pragma once


//Includes
#include "CoreMinimal.h"
#include "Core/Data/Enums/ActorType.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GameFramework/Actor.h"
#include "Machine.generated.h"
enum class EActorType : uint8;
class UHighlightComponent;
class AMachinePart;


//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMachineStatus);


UCLASS()
class COFFEESHOPGAME_API AMachine : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AMachine();

	
	//Delegates
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMachineStatus OnMachineActivated;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMachineStatus OnMachineDeactivated;

	
	//Methods
	UFUNCTION()
	virtual void TurnOn();

	UFUNCTION()
	virtual void TurnOff();
	
	UFUNCTION()
	bool IsOn();

	//Methods --> Getters
	UFUNCTION()
	UStaticMeshComponent* GetMeshComponent();	//hmmmmmmmmmmm --> shouldn't be here

protected:
	//Core Overrides
	virtual void BeginPlay() override;

	
	//Variables --> Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHighlightComponent* HighlightComponent = nullptr;
	
	
	//Variables --> Editable
	UPROPERTY(EditAnywhere, Category = "Settings")
	TMap<FName, TSubclassOf<AMachinePart>> MachineParts;
	
	//Variables --> Hidden
	UPROPERTY(Replicated)
	bool bIsOn = false;

	
	//Methods
	UFUNCTION()
	void SetupMachineParts();
	
	//Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual bool InteractStarted_Implementation(EActionId ActionId, FInteractionContext Context) override;
	
	//Interface-Used Methods
	UFUNCTION()
	void PickUpOrDrop(FInteractionContext Context);

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
