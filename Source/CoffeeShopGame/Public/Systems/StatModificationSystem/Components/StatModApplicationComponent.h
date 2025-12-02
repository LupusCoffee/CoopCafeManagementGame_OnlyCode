#pragma once

#include "CoreMinimal.h"
#include "StatHandlerComponent.h"
#include "Components/ActorComponent.h"
#include "Systems/StatModificationSystem/StatMod.h"
#include "StatModApplicationComponent.generated.h"


class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UStatModApplicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatModApplicationComponent();
	virtual void BeginPlay() override;

	//Methods
	UFUNCTION()
	void SetStatImpulse(UStatImpulse* StatImpulse);
	
	UFUNCTION()
	void SetStatMod(UStatMod* StatMod);
	
	UFUNCTION()
	void UpdateStatHandlers(bool bApplyStatMod);

protected:
	//Variables, Editable --> Static
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ApplicationRadius = 5000.0f;

	//Variables, Visible --> Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollider = nullptr;

	//Variables, Visible --> Static
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStatHandlerComponent*> RegisteredStatHandlers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStatHandlerComponent*> StatHandlersInRange;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStatHandlerComponent*> AppliedStatHandlers;

	//Variables, Visible --> Dynamic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatImpulse* CurrentStatImpulse = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatMod* CurrentStatMod = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatMod* PreviousStatMod = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bModShouldBeApplied = false;


	//Methods
	UFUNCTION()
	void CreateSphereCollider();
	
	UFUNCTION()
	void RegisterStatHandlersAndInitialOverlapCheck();

	UFUNCTION()
	void SubscribeToOverlapDelegates();
	
	UFUNCTION()
	void OnActorEnterRadius(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnActorExitRadius(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION()
	void OnStatHandlerEnterRadius(UStatHandlerComponent* StatHandlerComponent);

	UFUNCTION()
	void OnStatHandlerExitRadius(UStatHandlerComponent* StatHandlerComponent);

	UFUNCTION()
	void AddCurrentStatModToStatHandler(UStatHandlerComponent* StatHandlerComponent);

	UFUNCTION()
	void RemoveCurrentStatModFromStatHandler(UStatHandlerComponent* StatHandlerComponent);
};
