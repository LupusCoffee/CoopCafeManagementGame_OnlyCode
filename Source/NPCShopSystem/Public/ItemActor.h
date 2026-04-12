// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItemData.h"
#include "ItemActor.generated.h"

UCLASS()
class SHOPSYSTEM_API AItemActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	UBaseItemData* ItemData;
public:	
	// Sets default values for this actor's properties
	AItemActor();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ItemBase)
	bool Purchased = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemBase)
	UStaticMeshComponent* Mesh;
	
	UFUNCTION()
	void SetItemData() {}

	UFUNCTION()
	UBaseItemData* GetItemData() { return ItemData; }

	UFUNCTION()
	void SetMesh(UStaticMeshComponent* NewMesh);

	UFUNCTION()
	UStaticMeshComponent* GetMesh() { return Mesh; }
};
