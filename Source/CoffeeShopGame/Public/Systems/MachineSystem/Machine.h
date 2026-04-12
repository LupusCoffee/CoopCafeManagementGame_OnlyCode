#pragma once


//Includes
#include "CoreMinimal.h"
#include "Core/Data/Interfaces/Interactable.h"
#include "GameFramework/Actor.h"
#include "Machine.generated.h"
enum class EActorType : uint8;
class UHighlightComponent;
class AMachinePart;


//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMachineSetup, const TArray<AMachinePart*>&, MachineParts);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMachineStatus);


UCLASS()
class COFFEESHOPGAME_API AMachine : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AMachine();

	
	//Delegates
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMachineSetup OnMachineSetup;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMachineStatus OnMachineActivated;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMachineStatus OnMachineDeactivated;

	
	//Methods
	UFUNCTION()
	virtual void TurnOn();

	UFUNCTION()
	virtual void TurnOff();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOn();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AMachinePart*> GetMachineParts();

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
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsOn = false;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<AMachinePart*> InstancedMachineParts;

	
	//Methods
	UFUNCTION()
	void SetupMachineParts();
	

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	//Cosmetic Shiz
	virtual void Local_StartHover_Implementation(FPlayerContext Context) override;
	virtual void Local_EndHover_Implementation(FPlayerContext Context) override;
};
