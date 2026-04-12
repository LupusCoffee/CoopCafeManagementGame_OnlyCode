// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopGame/Public/Core/Data/Interfaces/Interactable.h"
#include "Chair.generated.h"

class ATable;

/**
 * A chair that can be sat in by AI characters.
 * Implements IInteractable for AI interaction.
 */
UCLASS()
class COFFEESHOPGAME_API AChair : public AActor, public IInteractable
{
public:
	

private:
	GENERATED_BODY()

public:
	AChair();
	
	//------------------------------------------------------------
	// OCCUPANCY API (used by TableManagerSubsystem)
	//------------------------------------------------------------
	
	/**
	 * Check if this chair is currently occupied
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	bool IsTaken() const;
	
	/**
	 * Get the actor currently using this chair
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	AActor* GetCurrentUser() const;
	
	/**
	 * Set who is using this chair (internal use by manager)
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	void SetCurrentUser(AActor* NewUser);
	
	/**
	 * Clear current user (leave the chair)
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	void LeaveChair();
	
	/**
	 * Get the position where an AI should move to interact with this chair
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	FVector GetSitPosition() const;
	
	/**
	 * Get the rotation the AI should face when sitting
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	FRotator GetSitRotation() const;
	
	//------------------------------------------------------------
	// TABLE ASSOCIATION
	//------------------------------------------------------------
	
	/**
	 * Get the table this chair belongs to
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	ATable* GetParentTable() const { return ParentTable; }
	
	/**
	 * Set the table this chair belongs to (called by table on registration)
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	void SetParentTable(ATable* Table) { ParentTable = Table; }
	
	/**
	 * Re-evaluate which table this chair should belong to
	 * Called when new tables are added to the world
	 */
	UFUNCTION(BlueprintCallable, Category = "Chair")
	void ReevaluateTableAssignment();
	
	//------------------------------------------------------------
	// IInteractable Interface
	//------------------------------------------------------------
	virtual void AIInteract_Implementation(APawn* Agent) override;
	
	USceneComponent* GetChairMovePoint() const { return ChairMovePoint; }
protected:
	virtual void BeginPlay() override;
	
	/** The actor currently sitting in this chair */
	UPROPERTY(BlueprintReadOnly, Category = "Chair")
	TWeakObjectPtr<AActor> CurrentUser;
	
	/** The table this chair belongs to */
	UPROPERTY(VisibleAnywhere, Category = "Chair")
	ATable* ParentTable = nullptr;
	
	/** Offset from chair center where AI should sit */
	UPROPERTY(EditAnywhere, Category = "Chair")
	FVector SitPositionOffset = FVector::ZeroVector;
	
	/** Whether to use chair's rotation or override it */
	UPROPERTY(EditAnywhere, Category = "Chair")
	bool bUseChairRotation = true;
	
	/** Override rotation if not using chair rotation */
	UPROPERTY(EditAnywhere, Category = "Chair", meta = (EditCondition = "!bUseChairRotation"))
	FRotator SitRotationOverride = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, Category = "Chair")
	USceneComponent* ChairMovePoint;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chair")
	UStaticMeshComponent* ChairMesh;
};
