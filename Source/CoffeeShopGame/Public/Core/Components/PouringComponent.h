#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PouringComponent.generated.h"
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UPouringComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPouringComponent();

	//Methods
	UFUNCTION()
	void StartVFX();

	UFUNCTION()
	void StopVFX();
	
protected:
	virtual void BeginPlay() override;

	//Variables --> Editable
	UPROPERTY(EditAnywhere, Category = "Pouring")
	UNiagaraSystem* PourVFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "Pouring")
	FName PourSocket = "";
	
	//Variables --> Hidden, Components
	UPROPERTY()
	UNiagaraComponent* PourVFXComp = nullptr;

	UPROPERTY()
	UStaticMeshComponent* OwnerMesh = nullptr;


	//Variables --> Replication
	UPROPERTY(ReplicatedUsing=OnRep_IsPouring)
	bool ForRep_bPourVfxIsOn = false;
	
	
	//Methods --> Replication
	// onrep
	UFUNCTION()
	void OnRep_IsPouring();
	
	// setup
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};