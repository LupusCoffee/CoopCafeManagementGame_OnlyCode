#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HeldItem.generated.h"

UCLASS(BlueprintType)
class UHeldItem : public UObject
{
	GENERATED_BODY()

public:
	UHeldItem() = default;

	//Init
	UFUNCTION()
	void Init(AActor* _Actor, UStaticMeshComponent* _MeshComp, UPrimitiveComponent* _PrimitiveComponent);

	//Getters
	UFUNCTION()
	AActor* GetActor();

	UFUNCTION()
	UStaticMeshComponent* GetMeshComp();

	UFUNCTION()
	UPrimitiveComponent* GetPrimitiveComponent();

	UFUNCTION()
	FCollisionResponseContainer GetInitialCollisionResponses();
	
	UFUNCTION()
	bool InitiallyHadPhysics();


protected:
	//Variables --> Replicated
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="HeldItem")
	AActor* Actor = nullptr;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="HeldItem")
	UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="HeldItem")
	UPrimitiveComponent* PrimitiveComponent = nullptr;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="HeldItem")
	FCollisionResponseContainer InitialCollisionResponses;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="HeldItem")
	bool bInitiallyHadPhysics = false;


	//Methods --> Replication
	//explanation: where you record properties of your object class that you want to replicate
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//explanation: informs replication system that references to this object class are supported for replication
	virtual bool IsSupportedForNetworking() const override;
};
