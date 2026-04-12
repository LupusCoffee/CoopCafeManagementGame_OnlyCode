#include "CoffeeShopGame/Public/Systems/ContainerSystem/Objects/ContainerObject.h"

#include "Net/UnrealNetwork.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Structs/ResourceAmount.h"



#pragma region Setup
UContainerObject::UContainerObject()
{
	CurrentTotalVolume = 0.f;
	MaxVolume = 0.f;
	bIsFull = false;

	bAcceptsAllResources = false;
	AcceptedResourceTypes = {};
	CurrentVolumeMap = {};
}

bool UContainerObject::SetupData(bool InAccpetsAllResources, TArray<EResourceType> InAcceptedResourceTypes,
	float InMaxVolume, TArray<FResourceAmount> StarterContents)
{	
	bAcceptsAllResources = InAccpetsAllResources;
	AcceptedResourceTypes = InAcceptedResourceTypes;
	MaxVolume = InMaxVolume;
	
	SetupResourceMap(AcceptedResourceTypes, StarterContents);
	
	return true;
}

void UContainerObject::SetupResourceMap(TArray<EResourceType> InAcceptedResourceTypes, TArray<FResourceAmount> InStarterContents)
{
	CurrentVolumeMap.Empty();
	
	for (auto ResourceType : InAcceptedResourceTypes)
	{
		CurrentVolumeMap.Add(FResourceAmount(ResourceType, 0));
	}
	
	for (auto ResourceStack : InStarterContents)
	{
		//add regardless of accepted resource types --> this is just what it starts with
		float ExcessAmount = 0.0f;
		AddVolume(ResourceStack.ResourceType, ResourceStack.Amount, ExcessAmount);
	}
}
#pragma endregion



#pragma region Volume: Adders, Removers, Setters, etc.
// adders
bool UContainerObject::TryAddVolume(EResourceType ResourceTypeToAdd, float Value, float& ExcessAmount, bool IgnoreAcceptedResourceTypes)
{
	if (!IgnoreAcceptedResourceTypes && !bAcceptsAllResources && !AcceptedResourceTypes.Contains(ResourceTypeToAdd)) return false;
	
	return AddVolume(ResourceTypeToAdd, Value, ExcessAmount);
}

bool UContainerObject::AddVolume(EResourceType ResourceTypeToAdd, float PreClampedValueToAdd, float& ExcessAmount)
{
	if (IsFull())
	{
		ExcessAmount = PreClampedValueToAdd;
		return false;
	}
	
	float MaxPossibleValueToAdd = MaxVolume-CurrentTotalVolume;
	float PostClampedValueToAdd = FMath::Clamp(PreClampedValueToAdd, 0.0f,  MaxPossibleValueToAdd);
	ExcessAmount = PreClampedValueToAdd - PostClampedValueToAdd;
	
	int ResourceIndex = FindByResourceTypeOrAdd(ResourceTypeToAdd);
	CurrentVolumeMap[ResourceIndex].Amount += PostClampedValueToAdd;

	UpdateTotalVolume();
	
	return true;
}

// removers
void UContainerObject::RemoveVolumeSequentially(float Value)
{
	if (CurrentTotalVolume <= 0.0f) return;

	float CurrentValueToRemove = Value;
	
	//todo: remove equal amount from all in current volume map!!
	for (auto& ResourceStack : CurrentVolumeMap)
	{
		if (CurrentValueToRemove <= 0.0f) break;
		
		float ClampedValueToRemove = FMath::Clamp(CurrentValueToRemove, 0.0f, ResourceStack.Amount);
		ResourceStack.Amount -= ClampedValueToRemove;
		CurrentValueToRemove -= ClampedValueToRemove;
	}

	UpdateTotalVolume();
}

//returns taken volume
TArray<FResourceAmount> UContainerObject::TakeVolumeSequentially(float Value)
{
	if (CurrentTotalVolume <= 0.0f) return TArray<FResourceAmount>();

	TArray<FResourceAmount> TakenVolume;

	float CurrentValueToRemove = Value;
	
	//todo: remove equal amount from all in current volume map!!
	for (auto& ResourceStack : CurrentVolumeMap)
	{
		if (CurrentValueToRemove <= 0.0f) break;
		
		float ClampedValueToRemove = FMath::Clamp(CurrentValueToRemove, 0.0f, ResourceStack.Amount);
		ResourceStack.Amount -= ClampedValueToRemove;
		CurrentValueToRemove -= ClampedValueToRemove;

		TakenVolume.Add(FResourceAmount(ResourceStack.ResourceType, ClampedValueToRemove));
	}
	
	UpdateTotalVolume();
	
	return TakenVolume;
}

void UContainerObject::RemoveSpecificVolume(EResourceType ResourceTypeToRemove, float Value, float& ExcessAmount)
{
	if (CurrentTotalVolume <= 0.0f) return;

	int ResourceIndex = FindByResourceTypeOrAdd(ResourceTypeToRemove);
	RemoveSpecificVolumeFromIndex(ResourceIndex, Value, ExcessAmount);
}

void UContainerObject::RemoveSpecificVolumeFromIndex(int ResourceIndexToRemoveFrom, float Value, float& ExcessAmount)
{
	if (CurrentTotalVolume <= 0.0f) return;
	if (ResourceIndexToRemoveFrom > CurrentVolumeMap.Num()-1) return;
	if (ResourceIndexToRemoveFrom < 0) return;
	
	float ClampedValueToRemove = FMath::Clamp(Value, 0.0f,  CurrentVolumeMap[ResourceIndexToRemoveFrom].Amount);
	CurrentVolumeMap[ResourceIndexToRemoveFrom].Amount -= ClampedValueToRemove;
	ExcessAmount = Value - ClampedValueToRemove;

	UpdateTotalVolume();
}

// setters
void UContainerObject::SetCurrentVolumeMap(TArray<FResourceAmount> InMap)
{
	CurrentVolumeMap = InMap;
	UpdateTotalVolume();
}

void UContainerObject::ZeroVolume()
{
	for (auto& ResourceStack : CurrentVolumeMap) ResourceStack.Amount = 0.0f;

	UpdateTotalVolume();
}

float UContainerObject::ZeroSpecificVolume(EResourceType ResourceTypeToZero)
{
	int i = FindByResourceTypeOrAdd(ResourceTypeToZero);
	int AmountRemoved = CurrentVolumeMap[i].Amount;
	CurrentVolumeMap[i].Amount = 0;

	UpdateTotalVolume();

	return AmountRemoved;
}

float UContainerObject::ZeroSpecificVolumeFromIndex(int ResourceIndexToZero)
{
	if (ResourceIndexToZero > CurrentVolumeMap.Num()-1) return -1;
	if (ResourceIndexToZero < 0) return -1;
	
	float AmountRemoved = CurrentVolumeMap[ResourceIndexToZero].Amount;
	CurrentVolumeMap[ResourceIndexToZero].Amount = 0;

	UpdateTotalVolume();

	return AmountRemoved;
}

// helpers/misc
int UContainerObject::FindByResourceTypeOrAdd(EResourceType ResourceToFindOrAdd)
{
	int ResourceIndex = -1;
	while (ResourceIndex == -1)
	{
		ResourceIndex = CurrentVolumeMap.IndexOfByPredicate([ResourceToFindOrAdd](const FResourceAmount& ResourceStack)
		{
			return ResourceStack.ResourceType == ResourceToFindOrAdd;
		});
		
		if (ResourceIndex == -1)
		{
			CurrentVolumeMap.Add(FResourceAmount(ResourceToFindOrAdd, 0));
		}
		else break;
	}

	return ResourceIndex;
}

void UContainerObject::UpdateTotalVolume()
{
	CurrentTotalVolume = 0;
	
	for (auto& ResourceStack : CurrentVolumeMap)
	{
		CurrentTotalVolume += ResourceStack.Amount;
	}
}
#pragma endregion



#pragma region Getters
bool UContainerObject::IsFull()
{
	if (CurrentTotalVolume >= MaxVolume) bIsFull = true;
	else bIsFull = false;

	if (bIsFull) GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, "FUUUUUUUUUUUUUUUUUUUUUUUUUULL");
	
	return bIsFull;
}

float UContainerObject::GetMaxVolume() const
{
	return MaxVolume;
}

float UContainerObject::GetCurrentTotalVolume() const
{
	return CurrentTotalVolume;
}

TArray<FResourceAmount> UContainerObject::GetCurrentVolumeMap() const
{
	return CurrentVolumeMap;
}

float UContainerObject::GetCurrentVolumeOfResource(EResourceType ResourceType)
{
	int ResourceIndex = CurrentVolumeMap.IndexOfByPredicate([ResourceType](const FResourceAmount& ResourceStack)
	{
		return ResourceStack.ResourceType == ResourceType;
	});
	if (ResourceIndex == -1) return -1;
	
	return CurrentVolumeMap[ResourceIndex].Amount;
}
#pragma endregion



#pragma region Replication
bool UContainerObject::IsSupportedForNetworking() const
{
	return true;
}

void UContainerObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UContainerObject, CurrentTotalVolume);
	DOREPLIFETIME(UContainerObject, CurrentVolumeMap);
	DOREPLIFETIME(UContainerObject, bIsFull);
}
#pragma endregion