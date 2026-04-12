// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoapWorldState.h"
#include "GameFramework/Actor.h"
#include "GoapPlanner.h"

// ===== Boolean State =====

void FGoapWorldState::SetBoolState(FName Key, bool Value)
{
	BoolStates.Add(Key, Value);
}

bool FGoapWorldState::GetBoolState(FName Key, bool DefaultValue) const
{
	if (const bool* Value = BoolStates.Find(Key))
	{
		return *Value;
	}
	return DefaultValue;
}

bool FGoapWorldState::HasBoolState(FName Key) const
{
	return BoolStates.Contains(Key);
}

void FGoapWorldState::RemoveBoolState(FName Key)
{
	BoolStates.Remove(Key);
}

// ===== Integer State =====

void FGoapWorldState::SetIntState(FName Key, int32 Value)
{
	IntStates.Add(Key, Value);
}

int32 FGoapWorldState::GetIntState(FName Key, int32 DefaultValue) const
{
	if (const int32* Value = IntStates.Find(Key))
	{
		return *Value;
	}
	return DefaultValue;
}

bool FGoapWorldState::HasIntState(FName Key) const
{
	return IntStates.Contains(Key);
}

void FGoapWorldState::RemoveIntState(FName Key)
{
	IntStates.Remove(Key);
}

// ===== Float State =====

void FGoapWorldState::SetFloatState(FName Key, float Value)
{
	FloatStates.Add(Key, Value);
}

float FGoapWorldState::GetFloatState(FName Key, float DefaultValue) const
{
	if (const float* Value = FloatStates.Find(Key))
	{
		return *Value;
	}
	return DefaultValue;
}

bool FGoapWorldState::HasFloatState(FName Key) const
{
	return FloatStates.Contains(Key);
}

void FGoapWorldState::RemoveFloatState(FName Key)
{
	FloatStates.Remove(Key);
}

// ===== Object State =====

void FGoapWorldState::SetObjectState(FName Key, UObject* Value)
{
	ObjectStates.Add(Key, Value);
}

UObject* FGoapWorldState::GetObjectState(FName Key) const
{
	if (const TObjectPtr<UObject>* Value = ObjectStates.Find(Key))
	{
		return *Value;
	}
	return nullptr;
}

bool FGoapWorldState::HasObjectState(FName Key) const
{
	return ObjectStates.Contains(Key);
}

void FGoapWorldState::RemoveObjectState(FName Key)
{
	ObjectStates.Remove(Key);
}

// ===== Actor State =====

void FGoapWorldState::SetActorState(FName Key, AActor* Value)
{
	SetObjectState(Key, Value);
}

AActor* FGoapWorldState::GetActorState(FName Key) const
{
	return Cast<AActor>(GetObjectState(Key));
}

bool FGoapWorldState::HasActorState(FName Key) const
{
	return HasObjectState(Key);
}

void FGoapWorldState::RemoveActorState(FName Key)
{
	RemoveObjectState(Key);
}

// ===== State Comparison & Utilities =====

bool FGoapWorldState::Satisfies(const FGoapWorldState& Other) const
{
	// Check if all boolean states in Other are satisfied
	for (const auto& Pair : Other.BoolStates)
	{
		bool CurrentValue = GetBoolState(Pair.Key, !Pair.Value);
		if (CurrentValue != Pair.Value)
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT("        ❌ Bool mismatch: %s - Required: %s, Current: %s"), 
				*Pair.Key.ToString(),
				Pair.Value ? TEXT("true") : TEXT("false"),
				CurrentValue ? TEXT("true") : TEXT("false"));
			return false;
		}
	}

	// Check if all integer states in Other are satisfied
	for (const auto& Pair : Other.IntStates)
	{
		int32 CurrentValue = GetIntState(Pair.Key, Pair.Value + 1);
		if (CurrentValue < Pair.Value)
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT("        ❌ Int mismatch: %s - Required: >=%d, Current: %d"), 
				*Pair.Key.ToString(), Pair.Value, CurrentValue);
			return false;
		}
	}

	// Check if all float states in Other are satisfied (with tolerance)
	for (const auto& Pair : Other.FloatStates)
	{
		float CurrentValue = GetFloatState(Pair.Key, Pair.Value + 1.0f);
		if (!FMath::IsNearlyEqual(CurrentValue, Pair.Value, GoapPlanningConstants::FLOAT_COMPARISON_TOLERANCE))
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT("        ❌ Float mismatch: %s - Required: %.2f, Current: %.2f"),
				*Pair.Key.ToString(), Pair.Value, CurrentValue);
			return false;
		}
	}

	// Check if all object states in Other are satisfied
	for (const auto& Pair : Other.ObjectStates)
	{
		UObject* ThisObject = GetObjectState(Pair.Key);
		UObject* OtherObject = Pair.Value;
		
		if (ThisObject != OtherObject)
		{
			FString ThisName = ThisObject ? ThisObject->GetName() : TEXT("nullptr");
			FString OtherName = OtherObject ? OtherObject->GetName() : TEXT("nullptr");
			UE_LOG(LogTemp, VeryVerbose, TEXT("        ❌ Object mismatch: %s - Required: %s, Current: %s"), 
				*Pair.Key.ToString(), *OtherName, *ThisName);
			return false;
		}
	}

	return true;
}

void FGoapWorldState::ApplyState(const FGoapWorldState& Effects)
{
	// Apply boolean effects
	for (const auto& Pair : Effects.BoolStates)
	{
		SetBoolState(Pair.Key, Pair.Value);
	}

	// Apply integer effects
	for (const auto& Pair : Effects.IntStates)
	{
		SetIntState(Pair.Key, Pair.Value);
	}

	// Apply float effects
	for (const auto& Pair : Effects.FloatStates)
	{
		SetFloatState(Pair.Key, Pair.Value);
	}

	// Apply object effects
	for (const auto& Pair : Effects.ObjectStates)
	{
		SetObjectState(Pair.Key, Pair.Value);
	}
}

int32 FGoapWorldState::GetDistanceTo(const FGoapWorldState& GoalState) const
{
	int32 Distance = 0;

	// Count mismatched boolean states
	for (const auto& Pair : GoalState.BoolStates)
	{
		if (GetBoolState(Pair.Key, !Pair.Value) != Pair.Value)
		{
			Distance++;
		}
	}

	// Count mismatched integer states
	for (const auto& Pair : GoalState.IntStates)
	{
		if (GetIntState(Pair.Key, Pair.Value + 1) < Pair.Value)
		{
			Distance++;
		}
	}

	// Count mismatched float states
	for (const auto& Pair : GoalState.FloatStates)
	{
		if (!FMath::IsNearlyEqual(GetFloatState(Pair.Key, Pair.Value + 1.0f), Pair.Value, GoapPlanningConstants::FLOAT_COMPARISON_TOLERANCE))
		{
			Distance++;
		}
	}

	// Count mismatched object states
	for (const auto& Pair : GoalState.ObjectStates)
	{
		if (GetObjectState(Pair.Key) != Pair.Value)
		{
			Distance++;
		}
	}

	return Distance;
}

void FGoapWorldState::Clear()
{
	BoolStates.Empty();
	IntStates.Empty();
	FloatStates.Empty();
	ObjectStates.Empty();
}

FString FGoapWorldState::ToString() const
{
	FString Result = TEXT("WorldState:\n");

	// Bool states
	if (BoolStates.Num() > 0)
	{
		Result += TEXT("  Bools:\n");
		for (const auto& Pair : BoolStates)
		{
			Result += FString::Printf(TEXT("    %s = %s\n"), 
				*Pair.Key.ToString(), 
				Pair.Value ? TEXT("true") : TEXT("false"));
		}
	}

	// Int states
	if (IntStates.Num() > 0)
	{
		Result += TEXT("  Ints:\n");
		for (const auto& Pair : IntStates)
		{
			Result += FString::Printf(TEXT("    %s = %d\n"), 
				*Pair.Key.ToString(), 
				Pair.Value);
		}
	}

	// Float states
	if (FloatStates.Num() > 0)
	{
		Result += TEXT("  Floats:\n");
		for (const auto& Pair : FloatStates)
		{
			Result += FString::Printf(TEXT("    %s = %.2f\n"), 
				*Pair.Key.ToString(), 
				Pair.Value);
		}
	}

	// Object states
	if (ObjectStates.Num() > 0)
	{
		Result += TEXT("  Objects:\n");
		for (const auto& Pair : ObjectStates)
		{
			UObject* Obj = Pair.Value;
			Result += FString::Printf(TEXT("    %s = %s\n"), 
				*Pair.Key.ToString(), 
				Obj ? *Obj->GetName() : TEXT("nullptr"));
		}
	}

	return Result;
}

bool FGoapWorldState::IsEmpty() const
{
	return BoolStates.Num() == 0 
		&& IntStates.Num() == 0 
		&& FloatStates.Num() == 0 
		&& ObjectStates.Num() == 0;
}

// ===== Operators =====

bool FGoapWorldState::operator==(const FGoapWorldState& Other) const
{
	// Compare bool states
	if (BoolStates.Num() != Other.BoolStates.Num())
	{
		return false;
	}
	for (const auto& Pair : BoolStates)
	{
		if (Other.GetBoolState(Pair.Key, !Pair.Value) != Pair.Value)
		{
			return false;
		}
	}

	// Compare int states
	if (IntStates.Num() != Other.IntStates.Num())
	{
		return false;
	}
	for (const auto& Pair : IntStates)
	{
		if (Other.GetIntState(Pair.Key, Pair.Value + 1) != Pair.Value)
		{
			return false;
		}
	}

	// Compare float states
	if (FloatStates.Num() != Other.FloatStates.Num())
	{
		return false;
	}
	for (const auto& Pair : FloatStates)
	{
		if (!FMath::IsNearlyEqual(Other.GetFloatState(Pair.Key, Pair.Value + 1.0f), Pair.Value, GoapPlanningConstants::FLOAT_COMPARISON_TOLERANCE))
		{
			return false;
		}
	}

	// Compare object states
	if (ObjectStates.Num() != Other.ObjectStates.Num())
	{
		return false;
	}
	for (const auto& Pair : ObjectStates)
	{
		if (Other.GetObjectState(Pair.Key) != Pair.Value)
		{
			return false;
		}
	}

	return true;
}


void FGoapWorldState::GetAllBoolKeys(TArray<FName>& OutKeys) const
{
	OutKeys.Empty();
	BoolStates.GetKeys(OutKeys);
}

void FGoapWorldState::GetAllIntKeys(TArray<FName>& OutKeys) const
{
	OutKeys.Empty();
	IntStates.GetKeys(OutKeys);
}

void FGoapWorldState::GetAllFloatKeys(TArray<FName>& OutKeys) const
{
	OutKeys.Empty();
	FloatStates.GetKeys(OutKeys);
}

void FGoapWorldState::GetAllObjectKeys(TArray<FName>& OutKeys) const
{
	OutKeys.Empty();
	ObjectStates.GetKeys(OutKeys);
}

void FGoapWorldState::DebugPrint(AActor* Owner) const
{
	FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");
	
	if (BoolStates.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Bool States:"));
		for (const auto& Pair : BoolStates)
		{
			UE_LOG(LogTemp, Warning, TEXT("    %s = %s"), *Pair.Key.ToString(), Pair.Value ? TEXT("true") : TEXT("false"));
		}
	}
	
	if (IntStates.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Int States:"));
		for (const auto& Pair : IntStates)
		{
			UE_LOG(LogTemp, Warning, TEXT("    %s = %d"), *Pair.Key.ToString(), Pair.Value);
		}
	}
	
	if (FloatStates.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Float States:"));
		for (const auto& Pair : FloatStates)
		{
			UE_LOG(LogTemp, Warning, TEXT("    %s = %.2f"), *Pair.Key.ToString(), Pair.Value);
		}
	}
	
	if (ObjectStates.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Object States:"));
		for (const auto& Pair : ObjectStates)
		{
			FString ObjName = Pair.Value ? Pair.Value->GetName() : TEXT("nullptr");
			UE_LOG(LogTemp, Warning, TEXT("    %s = %s"), *Pair.Key.ToString(), *ObjName);
		}
	}
	
	if (BoolStates.Num() == 0 && IntStates.Num() == 0 && FloatStates.Num() == 0 && ObjectStates.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  (empty)"));
	}
}

