// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GoapWorldState.generated.h"

USTRUCT(BlueprintType)
struct GOAPSYSTEM_API FGoapWorldState
{
	GENERATED_BODY()

	FGoapWorldState() = default;
	
	// Bool state
	void SetBoolState(FName Key, bool Value);
	bool GetBoolState(FName Key, bool DefaultValue = false) const;
	bool HasBoolState(FName Key) const;
	void RemoveBoolState(FName Key);

	// Int state
	void SetIntState(FName Key, int32 Value);
	int32 GetIntState(FName Key, int32 DefaultValue = 0) const;
	bool HasIntState(FName Key) const;
	void RemoveIntState(FName Key);

	// Float state
	void SetFloatState(FName Key, float Value);
	float GetFloatState(FName Key, float DefaultValue = 0.0f) const;
	bool HasFloatState(FName Key) const;
	void RemoveFloatState(FName Key);

	// Object state
	void SetObjectState(FName Key, UObject* Value);
	UObject* GetObjectState(FName Key) const;
	bool HasObjectState(FName Key) const;
	void RemoveObjectState(FName Key);

	// Actor state
	void SetActorState(FName Key, AActor* Value);
	AActor* GetActorState(FName Key) const;
	bool HasActorState(FName Key) const;
	void RemoveActorState(FName Key);

	// Core operations
	bool Satisfies(const FGoapWorldState& Other) const;
	void ApplyState(const FGoapWorldState& Effects);
	int32 GetDistanceTo(const FGoapWorldState& GoalState) const;
	void Clear();
	FString ToString() const;
	bool IsEmpty() const;
	void DebugPrint(AActor* Owner = nullptr) const;

	// Operators
	bool operator==(const FGoapWorldState& Other) const;
	bool operator!=(const FGoapWorldState& Other) const { return !(*this == Other); }

	// Direct state access
	const TMap<FName, bool>& GetBoolStates() const { return BoolStates; }
	const TMap<FName, int32>& GetIntStates() const { return IntStates; }
	const TMap<FName, float>& GetFloatStates() const { return FloatStates; }
	const TMap<FName, TObjectPtr<UObject>>& GetObjectStates() const { return ObjectStates; }

	void GetAllBoolKeys(TArray<FName>& OutKeys) const;
	void GetAllIntKeys(TArray<FName>& OutKeys) const;
	void GetAllFloatKeys(TArray<FName>& OutKeys) const;
	void GetAllObjectKeys(TArray<FName>& OutKeys) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World State")
	TMap<FName, bool> BoolStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World State")
	TMap<FName, int32> IntStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World State")
	TMap<FName, float> FloatStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World State")
	TMap<FName, TObjectPtr<UObject>> ObjectStates;
};

FORCEINLINE uint32 GetTypeHash(const FGoapWorldState& State)
{
	uint32 Hash = 0;
	
	TArray<FName> BoolKeys;
	State.GetBoolStates().GetKeys(BoolKeys);
	BoolKeys.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });
	for (const FName& Key : BoolKeys)
	{
		Hash = HashCombine(Hash, GetTypeHash(Key));
		Hash = HashCombine(Hash, GetTypeHash(State.GetBoolStates().FindRef(Key)));
	}

	TArray<FName> IntKeys;
	State.GetIntStates().GetKeys(IntKeys);
	IntKeys.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });
	for (const FName& Key : IntKeys)
	{
		Hash = HashCombine(Hash, GetTypeHash(Key));
		Hash = HashCombine(Hash, GetTypeHash(State.GetIntStates().FindRef(Key)));
	}

	TArray<FName> FloatKeys;
	State.GetFloatStates().GetKeys(FloatKeys);
	FloatKeys.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });
	for (const FName& Key : FloatKeys)
	{
		Hash = HashCombine(Hash, GetTypeHash(Key));
		Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(State.GetFloatStates().FindRef(Key) * 100.0f)));
	}

	TArray<FName> ObjectKeys;
	State.GetObjectStates().GetKeys(ObjectKeys);
	ObjectKeys.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });
	for (const FName& Key : ObjectKeys)
	{
		Hash = HashCombine(Hash, GetTypeHash(Key));
		Hash = HashCombine(Hash, GetTypeHash(State.GetObjectStates().FindRef(Key)));
	}

	return Hash;
}
