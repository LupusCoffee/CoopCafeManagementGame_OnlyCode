#include "Systems/Items/Components/ContainerComponent.h"
#include "Core/Data/Interfaces/Fillable.h"
#include "Core/Framework/Subsystems/LookTraceSubsystem.h"
#include "Net/UnrealNetwork.h"


//Constructor and Setup
UContainerComponent::UContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	
	//Setting Components & Subsystems
	// LookTraceSubsystem
	LookTraceSubsystem = GetWorld()->GetSubsystem<ULookTraceSubsystem>();

	// RecipeSubsystem
	RecipeSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<URecipeSubsystem>();
	
	
	//Setting Up Stats --> so that owners without a data asset can still initialize through their own data
	TryInitializeData();
}

bool UContainerComponent::SetupContainerData(UContainerData* ContainerData)
{
	if (!ContainerData) return false;
	
	//Container
	MaxVolume = ContainerData->MaxVolume;
	DecayVolumeOverTimeRate = ContainerData->DecayVolumeOverTimeRate;
	AcceptedResourceTypes = ContainerData->AcceptedResourceTypes;

	//Pouring
	HeightOverTargetUponPouringInto = ContainerData->HeightOverTargetUponPouringInto;
	PourDownTraceLength = ContainerData->PourDownTraceLength;
	PourDownTraceRadius = ContainerData->PourDownTraceRadius;
	PourDownTraceChannel = ContainerData->PourDownTraceChannel;
	PouringResourceType = ContainerData->PouringResource;

	TryInitializeData();
	
	return true;
}

bool UContainerComponent::TryInitializeData()
{
	if (bHasInitializedData) return false;
	
	SetupResourceMap();

	bHasInitializedData = true;
	
	return true;
}

void UContainerComponent::SetupResourceMap()
{
	CurrentVolumeMap.Empty();
	
	for (auto ResourceType : AcceptedResourceTypes)
	{
		CurrentVolumeMap.Add(FResourceStack(ResourceType, 0));
	}
	
	for (auto ResourceStack : StarterContents)
	{
		//add regardless of accepted resource types --> this is just what it starts with
		AddVolume(ResourceStack.ResourceType, ResourceStack.Amount);
	}
}


//Tick
void UContainerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DecayVolumeOverTimeRate > 0) RemoveVolume(DeltaTime * DecayVolumeOverTimeRate);
}


//Adders, Removers, Setters, etc.
bool UContainerComponent::TryAddVolume(EResourceType ResourceTypeToAdd, float Value)
{
	if (!AcceptedResourceTypes.Contains(ResourceTypeToAdd))
	{
		OnAddVolume.Broadcast(false, Value);
		return false;
	}
	
	return AddVolume(ResourceTypeToAdd, Value);
}

bool UContainerComponent::AddVolume(EResourceType ResourceTypeToAdd, float Value)
{
	if (IsFull())
	{
		OnAddVolume.Broadcast(false, Value);
		return false;
	}

	int ResourceIndex = CurrentVolumeMap.IndexOfByPredicate([ResourceTypeToAdd](const FResourceStack& ResourceStack)
	{
		return ResourceStack.ResourceType == ResourceTypeToAdd;
	});
	if (ResourceIndex == -1)
	{
		CurrentVolumeMap.Add(FResourceStack(ResourceTypeToAdd, 0));
	}

	float ClampedAddedValue = FMath::Clamp(Value, 0.0f,  MaxVolume-CurrentTotalVolume);
	CurrentVolumeMap[ResourceIndex].Amount += ClampedAddedValue;

	UpdateTotalVolume();
	UpdateCurrentRecipe();

	if (bBroadCastEmptyContainer) OnContainerNonEmpty.Broadcast(); bBroadCastEmptyContainer = false;	//hmmm

	OnAddVolume.Broadcast(true, Value);
	return true;
}

void UContainerComponent::RemoveVolume(float Value)
{
	if (CurrentTotalVolume <= 0.0f) return;

	float CurrentValueToRemove = Value;
	
	for (auto& ResourceStack : CurrentVolumeMap)
	{
		if (CurrentValueToRemove <= 0.0f) break;
		
		float ClampedValueToRemove = FMath::Clamp(CurrentValueToRemove, 0.0f, ResourceStack.Amount);
		ResourceStack.Amount -= ClampedValueToRemove;
		CurrentValueToRemove -= ClampedValueToRemove;
	}

	UpdateTotalVolume();
	UpdateCurrentRecipe();
	
	if (CurrentTotalVolume <= 0.0f && !bBroadCastEmptyContainer)
	{
		OnContainerEmpty.Broadcast();
		bBroadCastEmptyContainer = true;
	}
}

void UContainerComponent::ZeroVolume()
{
	for (auto ResourceStack : CurrentVolumeMap) ResourceStack.Amount = 0.0f;
	
	CurrentTotalVolume = 0.0f;
}

bool UContainerComponent::IsFull()
{
	if (CurrentTotalVolume >= MaxVolume) bIsFull = true;
	else bIsFull = false;

	if (bIsFull) GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, "FUUUUUUUUUUUUUUUUUUUUUUUUUULL");
	
	return bIsFull;
}

//Function is here so that AI can see if the item is avalible for them
bool UContainerComponent::IsAvailableForAI()
{
	if (IsFull() && !AIPickedUp)
	{
		return true;
	}
	
	return false;
}

//Function for the AI so they can mark a container as picked up for them
void UContainerComponent::AIPickUp(bool pickedUp)
{
	AIPickedUp = pickedUp;
}

void UContainerComponent::PourIntoMovedOverTargetStart(UHolderComponent* HolderCompForPourer, AActor* PourTargetActor, float InPourRate)
{
	//Null Checks and Setting
	if (!HolderCompForPourer) return;
	
	if (!PourTargetActor) return;

	AActor* MyPourer = GetOwner();
	if (!MyPourer) return;


	//Move Pourer
	// detatch
	HolderCompForPourer->DetachItemStillAsHeld(false);

	// set location
	FVector PourTargetLocation = PourTargetActor->GetActorLocation();

	FVector Origin, Extent;
	PourTargetActor->GetActorBounds(true, Origin, Extent);
	float HighestZ = Origin.Z + Extent.Z;
	
	FVector TopSurfaceOfTargetActor = FVector(PourTargetLocation.X, PourTargetLocation.Y, HighestZ);
	FVector PourLocation = TopSurfaceOfTargetActor + FVector::UpVector * HeightOverTargetUponPouringInto;
	
	MyPourer->SetActorLocation(PourLocation);


	//Set Pour Rate Multiplier
	SetPourRate(InPourRate);
}

void UContainerComponent::PourIntoMovedOverTargetEnd(UHolderComponent* HolderCompForPourer)
{
	if (!HolderCompForPourer) return;
	
	HolderCompForPourer->ReattachItemStillAsHeld();
}

void UContainerComponent::SetPourRate(float InPourRate)
{
	PourRate = InPourRate;
}

void UContainerComponent::PourDownTick(float DeltaTime)
{
	if (!GetOwner()) return;
	if (!LookTraceSubsystem) return;
	
	FHitResult Hit = LookTraceSubsystem->GetHitResultFromTargetSphereTrace(GetOwner(), FVector::DownVector,
		PourDownTraceLength, PourDownTraceRadius, PourDownTraceChannel);

	PourIntoTargetTick(DeltaTime, Hit.GetActor());
}

void UContainerComponent::PourIntoTargetTick(float DeltaTime, AActor* PourTarget)
{
	if (PouringResourceType == EResourceType::None) return;
	
	
	//Pour Volume from Container + Save Value
	float PouredVolume = DeltaTime * PourRate;
	float VolumeLeft = GetCurrentTotalVolume();
	
	float ClampedPouredVolume = FMath::Clamp(PouredVolume, 0.0f, VolumeLeft);
	
	RemoveVolume(ClampedPouredVolume);

	if (ClampedPouredVolume <= 0) return;

	
	//Add Volume to...
	if (!PourTarget || !PourTarget->GetClass()->ImplementsInterface(UFillable::StaticClass()))
	{
		OnPourIntoEmptySpot.Broadcast(ClampedPouredVolume);
	}
	else
	{
		IFillable::Execute_Recieve(PourTarget, PouringResourceType, ClampedPouredVolume);
	}
}


//Getters
float UContainerComponent::GetCurrentTotalVolume()
{
	return CurrentTotalVolume;
}

float UContainerComponent::GetCurrentVolume(EResourceType ResourceType)
{																																																																																					UE_LOG(LogTemp, Log, TEXT("I like boobies"));
	int ResourceIndex = CurrentVolumeMap.IndexOfByPredicate([ResourceType](const FResourceStack& ResourceStack)
	{
		return ResourceStack.ResourceType == ResourceType;
	});
	if (ResourceIndex == -1) return -1;
	
	return CurrentVolumeMap[ResourceIndex].Amount;
}

EResourceType UContainerComponent::GetPouringResourceType()
{
	return PouringResourceType;
}


//Helpers
void UContainerComponent::UpdateTotalVolume()
{
	CurrentTotalVolume = 0;
	
	for (auto& ResourceStack : CurrentVolumeMap)
	{
		CurrentTotalVolume += ResourceStack.Amount;
	}
}

void UContainerComponent::UpdateCurrentRecipe()
{
	if (!bConvertsResourcesToRecipe) return;


	//Update Current Recipe
	CurrentRecipe.Empty();
	
	for (auto& ResourceStack : CurrentVolumeMap)
	{		
		CurrentRecipe.Add(FCurrentResourcePercentage(ResourceStack.ResourceType,
			ResourceStack.Amount, MaxVolume));
	}

	
	//Update Content --> turn all resources into "resulting resource" if DiscoveredRecipe coincides
	if (!RecipeSubsystem) return;
	for (auto AvailableRecipe : RecipeSubsystem->GetAvailableRecipes())
	{
		for (auto Resource : AvailableRecipe->Recipe)
		{
			int ResourceIndex = CurrentRecipe.IndexOfByPredicate(
			[Resource](const FCurrentResourcePercentage& CurrentResourcePercentage)
			{
				return CurrentResourcePercentage.ResourceType == Resource.ResourceType;
			});
			
			if (ResourceIndex == -1) return;
			if (CurrentRecipe[ResourceIndex].CurrentPercentage < Resource.MinPercentage) return;
			if (CurrentRecipe[ResourceIndex].CurrentPercentage > Resource.MaxPercentage) return;
		}

		UpdateContent(AvailableRecipe->ResultingResource);
		RecipeSubsystem->TryAddDiscoveredRecipe(AvailableRecipe);
	}
}

void UContainerComponent::UpdateContent(EResourceType NewResourceType)
{
	CurrentVolumeMap.Empty();
	AddVolume(NewResourceType, CurrentTotalVolume);
}


//Replication
void UContainerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UContainerComponent, CurrentTotalVolume);
	DOREPLIFETIME(UContainerComponent, CurrentVolumeMap);
	DOREPLIFETIME(UContainerComponent, bIsFull);
}