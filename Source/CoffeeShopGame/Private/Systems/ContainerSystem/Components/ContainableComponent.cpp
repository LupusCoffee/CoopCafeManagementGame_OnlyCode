#include "CoffeeShopGame/Public/Systems/ContainerSystem/Components/ContainableComponent.h"
#include "NiagaraComponent.h"
#include "Characters/Components/StatHandlerCompatibleCharacterMovementComponent.h"
#include "Core/Framework/Subsystems/LookTraceSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Systems/Items/Actors/ContainerItem.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Enums/ResourceType.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Objects/ContainerObject.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Structs/ResourceAmount.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Subsystems/ResourceConverterSubsystem.h"
#include "Systems/StatModificationSystem/GameTags.h"

#if WITH_EDITOR
#include "Kismet2/BlueprintEditorUtils.h"
#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "TimerManager.h"
#endif

class UResourceConverterSubsystem;



//GENERAL ---------------------------------------------------------------------------------------------------------------------------------------------------------
//Setup
UContainableComponent::UContainableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	//Replication
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

// custom editor
#if WITH_EDITOR

static UBlueprint* GetOwningBlueprint(UActorComponent* Component)
{
    if (!Component) return nullptr;

    AActor* TempOwner = Component->GetOwner();
    if (!TempOwner) return nullptr;

    return Cast<UBlueprint>(TempOwner->GetClass()->ClassGeneratedBy);
}

void UContainableComponent::OnComponentCreated()
{
    Super::OnComponentCreated();
    QueueEnsurePourAnchorExists();
	QueueEnsurePourNiagaraExists();
}

void UContainableComponent::PostEditUndo()
{
    Super::PostEditUndo();
    QueueEnsurePourAnchorExists();
}

void UContainableComponent::QueueEnsurePourAnchorExists()
{
    if (bQueuedEnsurePourAnchor) return;

    UWorld* World = GetWorld();
    if (!World) return;

    bQueuedEnsurePourAnchor = true;

    World->GetTimerManager().SetTimerForNextTick([this]()
    {
        bQueuedEnsurePourAnchor = false;

        if (!IsValid(this)) return;

        EnsurePourAnchorExists();
    });
}

void UContainableComponent::EnsurePourAnchorExists()
{
    if (bIsEnsuringPourAnchor) return;
    bIsEnsuringPourAnchor = true;

    AActor* TempOwner = GetOwner();
    if (!TempOwner)
    {
        bIsEnsuringPourAnchor = false;
        return;
    }

    UBlueprint* Blueprint = GetOwningBlueprint(this);
    if (!Blueprint)
    {
        bIsEnsuringPourAnchor = false;
        return;
    }

    USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
    if (!SCS)
    {
        bIsEnsuringPourAnchor = false;
        return;
    }

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		if (!Node) continue;

		const FString Name = Node->GetVariableName().ToString();

		if (Name == TEXT("PourAnchor") || Name.StartsWith(TEXT("PourAnchor_")))
		{
			// Recover the name if we lost it
			PourAnchorName = Node->GetVariableName();

			bIsEnsuringPourAnchor = false;
			return;
		}
	}

    USCS_Node* NewNode = SCS->CreateNode(USceneComponent::StaticClass(), TEXT("PourAnchor"));
    if (!NewNode)
    {
        bIsEnsuringPourAnchor = false;
        return;
    }

    PourAnchorName = NewNode->GetVariableName();

    if (USCS_Node* RootNode = SCS->GetDefaultSceneRootNode())
    {
        RootNode->AddChildNode(NewNode);
    }
    else
    {
        SCS->AddNode(NewNode);
    }

    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

    bIsEnsuringPourAnchor = false;
}

void UContainableComponent::QueueEnsurePourNiagaraExists()
{
	if (bQueuedEnsurePourNiagara) return;

	UWorld* World = GetWorld();
	if (!World) return;

	bQueuedEnsurePourNiagara = true;

	World->GetTimerManager().SetTimerForNextTick([this]()
	{
		bQueuedEnsurePourNiagara = false;

		if (!IsValid(this)) return;

		EnsurePourNiagaraExists();
	});
}

void UContainableComponent::EnsurePourNiagaraExists()
{
	if (bIsEnsuringPourNiagara) return;
	bIsEnsuringPourNiagara = true;

	AActor* TempOwner = GetOwner();
	if (!TempOwner)
	{
		bIsEnsuringPourNiagara = false;
		return;
	}

	UBlueprint* Blueprint = GetOwningBlueprint(this);
	if (!Blueprint)
	{
		bIsEnsuringPourNiagara = false;
		return;
	}

	USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript;
	if (!SCS)
	{
		bIsEnsuringPourNiagara = false;
		return;
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		if (!Node) continue;

		const FString Name = Node->GetVariableName().ToString();

		if (Name == TEXT("PourVFX") || Name.StartsWith(TEXT("PourVFX_")))
		{
			PourNiagaraName = Node->GetVariableName();
			bIsEnsuringPourNiagara = false;
			return;
		}
	}

	USCS_Node* NewNode = SCS->CreateNode(UNiagaraComponent::StaticClass(), TEXT("PourVFX"));
	if (!NewNode)
	{
		bIsEnsuringPourNiagara = false;
		return;
	}

	PourNiagaraName = NewNode->GetVariableName();

	if (USCS_Node* RootNode = SCS->GetDefaultSceneRootNode())
	{
		RootNode->AddChildNode(NewNode);
	}
	else
	{
		SCS->AddNode(NewNode);
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	bIsEnsuringPourNiagara = false;
}
#endif

// begin play
void UContainableComponent::BeginPlay()
{
	Super::BeginPlay();

	
	//Get Owner + Mesh
	if (!Owner) Owner = GetOwner();
	if (!Owner) return;
	
	MeshComp = Owner->GetComponentByClass<UStaticMeshComponent>();
	

	//Get/Create Components
	TArray<USceneComponent*> Comps;
	Owner->GetComponents(Comps);
	for (USceneComponent* Comp : Comps)
	{
		// get PourAnchor
		if (Comp && Comp->GetName() == PourAnchorName)
		{
			PourAnchor = Comp;
		}
		// get Niagara Comp
		if (Comp && Comp->GetName() == PourNiagaraName)
		{
			NiagaraComponent = Cast<UNiagaraComponent>(Comp);
		}
	}
	
	if (!NiagaraComponent) NiagaraComponent = Owner->GetComponentByClass<UNiagaraComponent>();


	//Subsystems
	// LookTraceSubsystem
	LookTraceSubsystem = GetWorld()->GetSubsystem<ULookTraceSubsystem>();

	// RecipeSubsystem
	ResourceConverterSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UResourceConverterSubsystem>();

	
	//Delegates Subscribing
	OnStartPour.AddDynamic(this, &UContainableComponent::StartPourVisuals);
	OnEndPour.AddDynamic(this, &UContainableComponent::EndPourVisuals);
	OnContainerEmpty.AddDynamic(this, &UContainableComponent::EmptiedDuringPourVisuals);

	
	//Default Variables
	SetupVariables();
	SetupForReplicatedObjects();
	SetupContainerObject();
}

void UContainableComponent::SetupVariables()
{
	CurrentPourRate = BasePourRate;

	TrySetupVariablesWithContainerData();
}

bool UContainableComponent::TrySetupVariablesWithContainerData()
{
	if (!ContainerData) return false;

	//Container
	AcceptedResourceTypes = ContainerData->AcceptedResourceTypes;
	DecayVolumeRate = ContainerData->DecayVolumeOverTimeRate;

	//Pouring
	HeldPouringHeight = ContainerData->HeightOverTargetUponPouringInto;
	BasePouringResource = ContainerData->PouringResource;
	bConvertsContentsToRecipe = ContainerData->bConvertsResourcesToRecipe;
	PourDownTraceLength = ContainerData->PourDownTraceLength;
	PourDownTraceRadius = ContainerData->PourDownTraceRadius;
	PourDownTraceChannel = ContainerData->PourDownTraceChannel;

	//Volume
	MaxVolume = ContainerData->MaxVolume;
	
	return true;
}

void UContainableComponent::SetupContainerObject()
{
	if (!IsValid(ContainerObject)) return;
	ContainerObject->SetupData(bAcceptsAllResources, AcceptedResourceTypes, MaxVolume, StarterContents);
}


//Tick
void UContainableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Owner || !Owner->HasAuthority()) return;
	if (DecayVolumeRate > 0) RemoveVolumeSequentially(DeltaTime * DecayVolumeRate);
}



//INTERACTION ---------------------------------------------------------------------------------------------------------------------------------------------------------
//Pour Interaction
bool UContainableComponent::Server_StartInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext)
{
	if (ActionId != EActionId::LeftMouseButton) return false;
		
	return StartHeldPourInteraction(PlayerContext.Interactor, PlayerContext.HolderComponent,
		PlayerContext.Interactor->GetStatHandleableCharMoveComp(), PlayerContext.LookedAtActor);
}

bool UContainableComponent::Server_TickInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext, float DeltaTime)
{
	if (ActionId != EActionId::LeftMouseButton) return false;

	return TickHeldPourInteraction(PlayerContext.HolderComponent, PlayerContext.StatHandlerComponent, DeltaTime);
}

bool UContainableComponent::Server_EndInteraction_Implementation(EActionId ActionId, FPlayerContext PlayerContext)
{
	if (ActionId != EActionId::LeftMouseButton) return false;
	
	return EndHeldPourInteraction(PlayerContext.HolderComponent, PlayerContext.Interactor->GetStatHandleableCharMoveComp());
}

bool UContainableComponent::StartHeldPourInteraction(AActor* Interactor, UHolderComponent* HolderComp, UStatHandlerCompatibleCharacterMovementComponent* MovementComponent,
	AActor* PourTarget)
{
	if (!bCanPour) return false;
	if (!HolderComp) return false;
	if (!HolderComp->IsHoldingActor(Owner)) return false;

	if (MovementComponent && PourTarget) MovementComponent->BlockMovementAndJumpInput(true);
	
	TryMovePourerToTargetOrAnchor(HolderComp, Owner, PourTarget);
	
	OnStartPour.Broadcast(Interactor);
	
	return true;
}

bool UContainableComponent::TickHeldPourInteraction(UHolderComponent* HolderComp, UStatHandlerComponent* StatHandler, float DeltaTime)
{
	if (!HolderComp) return false;
	if (!HolderComp->IsHoldingActor(Owner)) return false;
	
	float PlayerPourRateMultiplier = 1;
	if (StatHandler) StatHandler->TryGetModifiedPropertyStat(TAG_PropertyStat_Player_PourRate, PlayerPourRateMultiplier);
	CurrentPourRate = BasePourRate * PlayerPourRateMultiplier;
	
	PourDownFromActor(Owner, DeltaTime);
	
	return true;
}

bool UContainableComponent::EndHeldPourInteraction(UHolderComponent* HolderComp, UStatHandlerCompatibleCharacterMovementComponent* MovementComponent)
{
	if (!HolderComp) return false;
	if (!HolderComp->IsHoldingActor(Owner)) return false;
	
	OnEndPour.Broadcast();

	ReturnPourerToHand(HolderComp);
	
	if (MovementComponent) MovementComponent->BlockMovementAndJumpInput(false);
	
	return true;
}


//Visuals
void UContainableComponent::StartPourVisuals_Implementation(AActor* PourInteractor)
{
	if (MeshComp)
	{
		InitialRotation = MeshComp->GetRelativeRotation();

		FVector LookAtPourInteractorDirection = (PourInteractor->GetActorLocation() - MeshComp->GetComponentLocation());
		FRotator LookAtPourInteractorRotation = LookAtPourInteractorDirection.Rotation();
		
		MeshComp->SetWorldRotation(FRotator(0, LookAtPourInteractorRotation.Yaw + 90.0f, 180.0f));
	}
	
	if (GetPouringResourceType() == EResourceType::None || GetCurrentTotalVolume() <= 0.0f) return;

	if (NiagaraComponent) NiagaraComponent->Activate();
}

void UContainableComponent::EndPourVisuals_Implementation()
{
	if (NiagaraComponent) NiagaraComponent->Deactivate();
	
	if (MeshComp) MeshComp->SetRelativeRotation(InitialRotation);
}

void UContainableComponent::EmptiedDuringPourVisuals_Implementation()
{
	if (NiagaraComponent) NiagaraComponent->Deactivate();
}



//UTILITIES ---------------------------------------------------------------------------------------------------------------------------------------------------------
//Pouring
void UContainableComponent::TryMovePourerToTargetOrAnchor(UHolderComponent* HolderComp, AActor* Pourer, AActor* PourTarget)
{
	//Null Checks and such
	if (!HolderComp) return;
	if (!Pourer) return;
	if (!PourTarget) return;

	UContainableComponent* TargetContainable = PourTarget->GetComponentByClass<UContainableComponent>();
	if (!TargetContainable) return;


	//Move Pourer
	// detatch
	HolderComp->DetachItemStillAsHeld();

	
	// set location
	USceneComponent* PourTargetAnchor = TargetContainable->GetPourAnchor();
	if (PourTargetAnchor)
	{
		Pourer->SetActorLocation(PourTargetAnchor->GetComponentLocation() + FVector::UpVector * HeldPouringHeight);
	}
	else
	{
		MovePourerToTarget(Pourer, PourTarget);
	}


	//Cache Pour Target
	PourTargetAsMovedOverObjectCached = PourTarget;
}
void UContainableComponent::MovePourerToTarget(AActor* Pourer, AActor* PourTarget)
{
	FVector PourTargetLocation = PourTarget->GetActorLocation();

	FVector Origin, Extent;
	PourTarget->GetActorBounds(true, Origin, Extent);
	float HighestZ = Origin.Z + Extent.Z;
	
	FVector TopSurfaceOfTargetActor = FVector(PourTargetLocation.X, PourTargetLocation.Y, HighestZ);
	FVector PourLocation = TopSurfaceOfTargetActor + FVector::UpVector * HeldPouringHeight;
	
	Pourer->SetActorLocation(PourLocation);
}

void UContainableComponent::ReturnPourerToHand(UHolderComponent* HolderComp)
{
	if (HolderComp) HolderComp->ReattachItemStillAsHeld();

	if (!PourTargetAsMovedOverObjectCached) return;
	
	UContainableComponent* ContainerComp = PourTargetAsMovedOverObjectCached->GetComponentByClass<UContainableComponent>();
	if (ContainerComp) ContainerComp->TryMixContents();
	
	PourTargetAsMovedOverObjectCached = nullptr;
}

void UContainableComponent::PourDownFromActor(AActor* InActor, float DeltaTime)
{
	if (!LookTraceSubsystem) return;
	if (!InActor) return;
	if (!bCanPour) return;
	
	FHitResult Hit = LookTraceSubsystem->GetHitResultFromTargetSphereTrace(InActor, FVector::DownVector,
		PourDownTraceLength, PourDownTraceRadius, PourDownTraceChannel);

	PourIntoTarget(Hit.GetActor(), DeltaTime);
}

void UContainableComponent::PourIntoTarget(AActor* PourTarget, float DeltaTime)
{
	//Return Check
	if (!bCanPour) return;
	
	
	//Get Resource Type to Pour
	EResourceType ResourceTypeToPour = GetPouringResourceType();
	if (ResourceTypeToPour == EResourceType::None) return;
	
	
	//Get Volume Amount
	float PouredVolume = DeltaTime * CurrentPourRate;
	float VolumeLeft = GetCurrentTotalVolume();
	
	float ClampedPouredVolume = FMath::Clamp(PouredVolume, 0.0f, VolumeLeft);
	RemoveVolumeSequentially(ClampedPouredVolume);
	if (ClampedPouredVolume <= 0) return;


	//Pour Volume Amount
	// pour failed broadcast lambda
	auto PourFailed = [&](){ OnAddVolume.Broadcast(false, ClampedPouredVolume); };

	// if no viable target
	if (!PourTarget) {PourFailed(); return;}
	if (PourTargetCached != PourTarget)
	{
		PourTargetCached = PourTarget;
		PourTargetContainerCached = PourTargetCached->GetComponentByClass<UContainableComponent>();
	}
	if (!PourTargetContainerCached || !PourTargetContainerCached->bCanBePouredInto) {PourFailed(); return;}
	
	// if viable target
	float ExcessAmount = 0.0f;
	PourTargetContainerCached->TryAddVolume(ResourceTypeToPour, ClampedPouredVolume, ExcessAmount, false);
}

EResourceType UContainableComponent::GetPouringResourceType()
{
	//if we dont convert content, return set pouring resource type
	if (!bUseContentsAsPouringResource) return BasePouringResource;

	
	//if we DO convert content, get resource of which there is most of and return it
	float HighestResourceAmount = 0;
	EResourceType HighestResourceType = BasePouringResource;
		
	for (auto ResourceAmount : GetCurrentVolumeMap())
	{
		if (ResourceAmount.Amount > HighestResourceAmount)
		{
			HighestResourceAmount = ResourceAmount.Amount;
			HighestResourceType = ResourceAmount.ResourceType;
		}
	}
		
	return HighestResourceType;
}

USceneComponent* UContainableComponent::GetPourAnchor() const
{
	return PourAnchor;
}


//Conversion
bool UContainableComponent::TryMixContents()
{
	if (!bConvertsContentsToRecipe) return false;
	
	TArray<FResourceAmount> ConvertedContent = GetCurrentVolumeMap();
	if (!ResourceConverterSubsystem->TryConvertFromRecipe(ConvertedContent)) return false;
	
	SetCurrentVolumeMap(ConvertedContent);
	return true;
}

void UContainableComponent::TryGrindContents()
{
	for (int i = 0; i < GetCurrentVolumeMap().Num(); ++i)
	{
		EResourceType PreConversionResource = GetCurrentVolumeMap()[i].ResourceType;
		EResourceType PostConversionResource = ResourceConverterSubsystem->TryGetGrindConversionOutputResource(PreConversionResource);
		if (PostConversionResource == EResourceType::None) continue;

		float AmountRemoved = EmptySpecificVolumeFromIndex(i);
		float ExcessAmount = 0.0f;
		TryAddVolume(PostConversionResource, AmountRemoved, ExcessAmount, false);
	}
}


//Volume
// add / remove
bool UContainableComponent::TryAddVolume(EResourceType ResourceTypeToAdd, float Value, float& ExcessAmount, bool IgnoreAcceptedResourceTypes)
{
	if (!Owner || !Owner->HasAuthority()) return false;
	
	bool ResultStatus = ContainerObject->TryAddVolume(ResourceTypeToAdd, Value, ExcessAmount, IgnoreAcceptedResourceTypes);
    
	if (!ResultStatus) OnAddVolume.Broadcast(false, Value);
	else OnAddVolume.Broadcast(true, Value);

	if (bEmptyContainerBroadcasted)
	{
		OnContainerNotEmpty.Broadcast();
		bEmptyContainerBroadcasted = false;
	}
    
	return ResultStatus;
}

void UContainableComponent::RemoveVolumeSequentially(float Value)
{
	if (!Owner || !Owner->HasAuthority()) return;
	
	ContainerObject->RemoveVolumeSequentially(Value);
	
	if (GetCurrentTotalVolume() <= 0.0f && !bEmptyContainerBroadcasted)
	{
		OnContainerEmpty.Broadcast();
		bEmptyContainerBroadcasted = true;
	}
}

// set / empty
void UContainableComponent::SetCurrentVolumeMap(TArray<FResourceAmount> InMap)
{
	if (!Owner || !Owner->HasAuthority()) return;
	
	ContainerObject->SetCurrentVolumeMap(InMap);
}

void UContainableComponent::EmptyVolume()
{
	if (!Owner || !Owner->HasAuthority()) return;
	
	ContainerObject->ZeroVolume();
}

float UContainableComponent::EmptySpecificVolume(EResourceType ResourceTypeToZero)
{
	if (!Owner || !Owner->HasAuthority()) return -1;
	
	return ContainerObject->ZeroSpecificVolume(ResourceTypeToZero);
}

float UContainableComponent::EmptySpecificVolumeFromIndex(int ResourceIndexToZero)
{
	if (!Owner || !Owner->HasAuthority()) return -1;
	
	return ContainerObject->ZeroSpecificVolumeFromIndex(ResourceIndexToZero);
}

// getters
UContainerObject* UContainableComponent::GetContainerObject()
{
	return ContainerObject;
}

bool UContainableComponent::IsFull()
{
	if (!IsValid(ContainerObject)) return 0.f;
	
	return ContainerObject->IsFull();
}

float UContainableComponent::GetMaxVolume()
{
	if (!IsValid(ContainerObject)) return 0.f;
	
	return ContainerObject->GetMaxVolume();
}

float UContainableComponent::GetCurrentTotalVolume()
{
	if (!IsValid(ContainerObject)) return 0.f;
	
	return ContainerObject->GetCurrentTotalVolume();
}

TArray<FResourceAmount> UContainableComponent::GetCurrentVolumeMap()
{
	if (!IsValid(ContainerObject)) return TArray<FResourceAmount>();
	
	return ContainerObject->GetCurrentVolumeMap();
}

float UContainableComponent::GetCurrentVolumeOfResource(EResourceType ResourceType)
{
	if (!IsValid(ContainerObject)) return 0.f;
	
	return ContainerObject->GetCurrentVolumeOfResource(ResourceType);
}



//REPLICATION ---------------------------------------------------------------------------------------------------------------------------------------------------------
void UContainableComponent::SetupForReplicatedObjects()
{	
	if (!Owner|| !Owner->HasAuthority()) return;

	if (IsValid(ContainerObject))
	{
		RemoveReplicatedSubObject(ContainerObject);
		ContainerObject = nullptr;
	}

	ContainerObject = NewObject<UContainerObject>(this);
	AddReplicatedSubObject(ContainerObject);

	Owner->ForceNetUpdate();
}

void UContainableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UContainableComponent, ContainerObject);
}