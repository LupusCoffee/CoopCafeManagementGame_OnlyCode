// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Core/GoapWorldState.h"
#include "GoapGoalDataAsset.generated.h"

UENUM(BlueprintType)
enum class EGoapComparisonOperator : uint8
{
	Equal UMETA(DisplayName = "Equal (==)"),
	NotEqual UMETA(DisplayName = "Not Equal (!=)"),
	GreaterThan UMETA(DisplayName = "Greater Than (>)"),
	GreaterOrEqual UMETA(DisplayName = "Greater or Equal (>=)"),
	LessThan UMETA(DisplayName = "Less Than (<)"),
	LessOrEqual UMETA(DisplayName = "Less or Equal (<=)")
};

USTRUCT(BlueprintType)
struct GOAPSYSTEM_API FGoapFloatCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName StateKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	EGoapComparisonOperator Operator = EGoapComparisonOperator::GreaterThan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float Value = 0.0f;

	FGoapFloatCondition()
		: StateKey(NAME_None)
		, Operator(EGoapComparisonOperator::GreaterThan)
		, Value(0.0f)
	{}

	FGoapFloatCondition(FName InKey, EGoapComparisonOperator InOperator, float InValue)
		: StateKey(InKey)
		, Operator(InOperator)
		, Value(InValue)
	{}

	bool Evaluate(const FGoapWorldState& WorldState) const;
};

USTRUCT(BlueprintType)
struct GOAPSYSTEM_API FGoapIntCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName StateKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	EGoapComparisonOperator Operator = EGoapComparisonOperator::GreaterThan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	int32 Value = 0;

	FGoapIntCondition()
		: StateKey(NAME_None)
		, Operator(EGoapComparisonOperator::GreaterThan)
		, Value(0)
	{}

	FGoapIntCondition(FName InKey, EGoapComparisonOperator InOperator, int32 InValue)
		: StateKey(InKey)
		, Operator(InOperator)
		, Value(InValue)
	{}

	bool Evaluate(const FGoapWorldState& WorldState) const;
};

USTRUCT(BlueprintType)
struct GOAPSYSTEM_API FGoapBoolCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName StateKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bDesiredValue = true;

	FGoapBoolCondition()
		: StateKey(NAME_None)
		, bDesiredValue(true)
	{}

	FGoapBoolCondition(FName InKey, bool InValue)
		: StateKey(InKey)
		, bDesiredValue(InValue)
	{}

	bool Evaluate(const FGoapWorldState& WorldState) const;
};

/** GOAP goal template - READ ONLY at runtime */
UCLASS(BlueprintType)
class GOAPSYSTEM_API UGoapGoalDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UGoapGoalDataAsset();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Goal")
	FText GoalName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Goal")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning")
	float BasePriority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Desired State", meta = (DisplayName = "Bool States"))
	TMap<FName, bool> DesiredBoolStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Desired State", meta = (DisplayName = "Object States"))
	TMap<FName, TObjectPtr<UObject>> DesiredObjectStates;

	/** Operator-based desired int states (key, operator, value) shown under Desired State. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Desired State", meta = (DisplayName = "Int Desired Conditions"))
	TArray<FGoapIntCondition> IntDesiredConditions;

	/** Operator-based desired float states (key, operator, value) shown under Desired State. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Desired State", meta = (DisplayName = "Float Desired Conditions"))
	TArray<FGoapFloatCondition> FloatDesiredConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Validation", meta = (DisplayName = "Float Validation Conditions"))
	TArray<FGoapFloatCondition> FloatValidationConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Validation", meta = (DisplayName = "Int Validation Conditions"))
	TArray<FGoapIntCondition> IntValidationConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planning|Validation", meta = (DisplayName = "Bool Validation Conditions"))
	TArray<FGoapBoolCondition> BoolValidationConditions;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GOAP")
	float GetPriority(AActor* Agent, const FGoapWorldState& CurrentState) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GOAP")
	bool IsValid(AActor* Agent, const FGoapWorldState& CurrentState) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GOAP")
	FGoapWorldState GetDesiredState(AActor* Agent, const FGoapWorldState& CurrentState) const;

protected:
	virtual float GetPriority_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const;
	virtual bool IsValid_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const;
	virtual FGoapWorldState GetDesiredState_Implementation(AActor* Agent, const FGoapWorldState& CurrentState) const;
};
