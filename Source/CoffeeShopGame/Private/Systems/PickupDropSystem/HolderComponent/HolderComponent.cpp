#include "Systems/PickupDropSystem/HolderComponent/HolderComponent.h"
#include "Core/Framework/Subsystems/LookTraceSubsystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
class ULookTraceSubsystem;

//Big ol' anime tiddies

//Constructors, Overrides, Init
UHolderComponent::UHolderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//Replication
	bReplicateUsingRegisteredSubObjectList = true;
}

void UHolderComponent::BeginPlay()
{
	Super::BeginPlay();

	
	Pawn = Cast<APawn>(GetOwner());
	if (Pawn) TryInitTraceSubsystem();
	
	
	ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
	
	if (OwningCharacter) OwnerMesh = OwningCharacter->GetMesh();
	
	if (!OwnerMesh) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red,
		   FString::Printf(TEXT("Owner mesh in HolderComponent on actor '%s' is null"), *GetOwner()->GetName()));


	BeginPlaySetupForReplicatedObjects();
}

bool UHolderComponent::TryInitTraceSubsystem()
{
	if (!Pawn || !Pawn->IsLocallyControlled()) return false;

	Controller = Pawn->GetController();
	if (!Controller) return false;

	LookTraceSubsystem = GetWorld()->GetSubsystem<ULookTraceSubsystem>();
	if (!LookTraceSubsystem) return false;
	
	return true;
}

void UHolderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bClientSideWantsToTickThrow)
		ClientSide_ThrowItemTick(DeltaTime);
}


//Public --> Pick Up
void UHolderComponent::PickUpItem(UHeldItem* ItemToPickUp)
{
	//return checks
	if (!OwnerMesh) return;
	if (bIsHolding) return;
	if (HeldItem) return;
	
	HeldItem = ItemToPickUp;
	if (!HeldItem) return;

	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return;

	//turn off physics if it has it - todo: make into function?
	if (UPrimitiveComponent* PrimitiveComponent = HeldItem->GetPrimitiveComponent())
	{
		if (HeldItem->InitiallyHadPhysics())
		{
			PrimitiveComponent->SetSimulatePhysics(false);
		}

		// Turn off collision regardless whether they had physics or not
		Multicast_UpdatePrimitiveComponentCollision(PrimitiveComponent, ECollisionEnabled::NoCollision);
	}
	
	//move item actor to HeldItemSocke
	if (!HeldItemActor->AttachToComponent(
		OwnerMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		HeldItemSocketName
	))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red,
		   FString::Printf(TEXT("Attach Failed %s"), *GetOwner()->GetName()));
	}

	OnAttachItem.Broadcast(HeldItem);
	bIsHolding = true;
	bIsAttaching = false;
}


//Public --> Drop
void UHolderComponent::DropItem(FVector InLookedAtLocation)
{
	FVector LookedAtLocation = InLookedAtLocation;
	if (LookedAtLocation == FVector::ZeroVector) return;
	
	if (!HeldItem) return;
	
	//detach and move to looked at location
	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return;
	
	HeldItemActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HeldItemActor->SetActorLocation(LookedAtLocation);
	
	//turn off physics if it had it - todo: make into function?
	if (UPrimitiveComponent* PrimitiveComponent = HeldItem->GetPrimitiveComponent())
	{
		if (HeldItem->InitiallyHadPhysics())
		{
			PrimitiveComponent->SetSimulatePhysics(true);
		}

		// Turn off collision regardless whether they had physics or not
		Multicast_UpdatePrimitiveComponentCollision(PrimitiveComponent, ECollisionEnabled::QueryAndPhysics);
	}

	OnDetachItem.Broadcast(HeldItem);
	
	HeldItem = nullptr;
	bIsHolding = false;
	bIsAttaching = false;
}

void UHolderComponent::AttachItemToSocket(UStaticMeshComponent* MeshWithSocket, FName SocketName)
{
	if (!HeldItem) return;

	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return;
	
	HeldItemActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//todo: switch to attach to actor
	HeldItemActor->AttachToComponent(
		MeshWithSocket,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);

	OnDetachItem.Broadcast(HeldItem);
	
	HeldItem = nullptr;
	bIsHolding = false;
	bIsAttaching = true;
}

void UHolderComponent::DetachItemStillAsHeld(bool bResetPhysicsToInitialState)
{
	if (!HeldItem) return;

	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return;

	//turn on physics if it initially had it - todo: make into function?
	if (UPrimitiveComponent* PrimitiveComponent = HeldItem->GetPrimitiveComponent())
	{
		if (HeldItem->InitiallyHadPhysics())
		{
			PrimitiveComponent->SetSimulatePhysics(true);
		}

		// Turn off collision regardless whether they had physics or not
		Multicast_UpdatePrimitiveComponentCollision(PrimitiveComponent, ECollisionEnabled::NoCollision);
	}

	HeldItemActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void UHolderComponent::ReattachItemStillAsHeld()
{
	//return checks
	if (!HeldItem) return;

	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return;

	//turn off physics if it has it - todo: make into function?
	if (UPrimitiveComponent* PrimitiveComponent = HeldItem->GetPrimitiveComponent())
	{
		if (HeldItem->InitiallyHadPhysics())
		{
			PrimitiveComponent->SetSimulatePhysics(false);
		}

		// Turn off collision regardless whether they had physics or not
		Multicast_UpdatePrimitiveComponentCollision(PrimitiveComponent, ECollisionEnabled::NoCollision);
	}
	
	//move item actor to HeldItemSocke
	if (!HeldItemActor->AttachToComponent(
		OwnerMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		HeldItemSocketName
	))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red,
		   FString::Printf(TEXT("Attach Failed %s"), *GetOwner()->GetName()));
	}
}


//Public --> Throw
void UHolderComponent::ServerSide_ThrowItemStart()
{
	if (!HeldItem) return;
	if (!HeldItem->InitiallyHadPhysics()) return;

	if (GetOwner()->HasAuthority()) Client_ThrowItemStart();
	
	ServerSideStartThrowTimeStamp = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void UHolderComponent::Client_ThrowItemStart_Implementation()
{
	ClientSideCurrentThrowForce = 0.0f;
	bClientSideWantsToTickThrow = true;

	OwnerClient_OnThrowStart.Broadcast();
}

void UHolderComponent::ClientSide_ThrowItemTick(float DeltaTime)
{
	ClientSideCurrentThrowForce += DeltaTime * ThrowForceMultiplier;
	
	if (ClientSideCurrentThrowForce > MaxThrowForce) ClientSideCurrentThrowForce = MaxThrowForce;
	if (ClientSideCurrentThrowForce < MinThrowForce) ClientSideCurrentThrowForce = MinThrowForce;

	OwnerClient_OnThrowItemTick.Broadcast(ClientSideCurrentThrowForce, MinThrowForce, MaxThrowForce);
}

void UHolderComponent::ServerSide_ThrowItemCompleted(AController* InController)
{
	float TimeElapsed = GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - ServerSideStartThrowTimeStamp;
	float ThrowForce = TimeElapsed * ThrowForceMultiplier;
	
	ThrowItem(InController, ThrowForce);
	
	if (GetOwner()->HasAuthority()) Client_ThrowItemCompleted();
}

void UHolderComponent::Client_ThrowItemCompleted_Implementation()
{
	bClientSideWantsToTickThrow = false;

	OwnerClient_OnThrowEnd.Broadcast();
}

void UHolderComponent::ThrowItem(AController* _Controller, float ThrowForce)
{
	if (!HeldItem) return;
	if (!HeldItem->InitiallyHadPhysics()) return;
	if (!_Controller) return;

	FVector ThrowDir = GetThrowDir(_Controller);
	
	AActor* HeldItemActor = HeldItem->GetActor();
	if (!HeldItemActor) return;

	UPrimitiveComponent* PrimitiveComponent = HeldItem->GetPrimitiveComponent();
	if (!PrimitiveComponent) return;

	//Detach
	HeldItemActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//Set Location Before Throw
	FVector PreThrowLoc = GetPreThrowLocation(HeldItemActor, _Controller); //hmmmm --> todo: dont wanna calculate this on server
	HeldItemActor->SetActorLocation(PreThrowLoc);
	
	//turn on physics
	PrimitiveComponent->SetSimulatePhysics(true);
	PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//Set throw force
	float UsedThrowForce = ThrowForce * PrimitiveComponent->GetMass();
	
	//throw
	PrimitiveComponent->AddImpulse(ThrowDir * UsedThrowForce);

	OnDetachItem.Broadcast(HeldItem);
	
	HeldItem = nullptr;
	bIsHolding = false;
	bIsAttaching = false;
}

FVector UHolderComponent::GetThrowDir(AController* _Controller)
{
	//calculate view location and rotation
	FVector ViewLocation;
	FRotator ViewRotation;
	_Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	FVector LookedAtDirection = ViewRotation.Vector();

	//set throw dir
	FRotator YawRot(0.f, ViewRotation.Yaw, 0.f);
	FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	FVector ThrowDir = FQuat(Right, FMath::DegreesToRadians(-ThrowAngle)).RotateVector(LookedAtDirection);

	return ThrowDir;
}

FVector UHolderComponent::GetPreThrowLocation(AActor* HeldItemActor, AController* _Controller)
{
	if (!_Controller) return FVector::ZeroVector;
	
	//calculate view location and rotation
	FVector ViewLocation;
	FRotator ViewRotation;
	_Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	FVector LookedAtDirection = ViewRotation.Vector();

	//set start pos - todo: set it to right in front of camera always (or??)
	float StartPosX = ViewLocation.X + LookedAtDirection.X * ForwardStartPos;
	float StartPosY = ViewLocation.Y + LookedAtDirection.Y * ForwardStartPos;
	float StartPosZ = HeldItemActor->GetActorLocation().Z;
	FVector ThrowStartLoc = FVector(StartPosX, StartPosY, StartPosZ);
	
	return ThrowStartLoc;
}


//Getters
FVector UHolderComponent::GetLookedAtSurfaceLocation()	//might wanna do all of this from a single component?
{
	if (!LookTraceSubsystem) return FVector::ZeroVector;

	if (!Controller) return FVector::ZeroVector;
	
	return LookTraceSubsystem->GetLocationFromLineTrace(Controller);
}

UHeldItem* UHolderComponent::GetHeldItem()
{
	return HeldItem;
}

bool UHolderComponent::IsHolding()
{
	return bIsHolding;
}


//Replication
// multicast
void UHolderComponent::Multicast_UpdatePrimitiveComponentCollision_Implementation(UPrimitiveComponent* PrimitiveComponent, ECollisionEnabled::Type Col)
{
	PrimitiveComponent->SetCollisionEnabled(Col);
}

// onrep
void UHolderComponent::OnRep_HeldItemUpdate(UHeldItem* LastHeldItem)
{
	UHeldItem* ResultHeldItem = nullptr;

	if (HeldItem) ResultHeldItem = HeldItem;
	else ResultHeldItem = LastHeldItem;
	
	if (!ResultHeldItem->InitiallyHadPhysics()) return;
	
	UPrimitiveComponent* PrimitiveComponent = ResultHeldItem->GetPrimitiveComponent();
	if (!PrimitiveComponent) return;
	
	if (bIsHolding)
	{
		PrimitiveComponent->SetSimulatePhysics(false);
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		if (bIsAttaching) return;	//hmmmmm
		PrimitiveComponent->SetSimulatePhysics(true);
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

// setup
void UHolderComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHolderComponent, HeldItem);
	DOREPLIFETIME(UHolderComponent, bIsHolding);
	DOREPLIFETIME(UHolderComponent, bIsAttaching);
}

void UHolderComponent::BeginPlaySetupForReplicatedObjects()
{
	//create replicated HeldItem UObject on Server
	
	if (!GetOwner()->HasAuthority()) return;

	if (IsValid(HeldItem)) RemoveReplicatedSubObject(HeldItem);

	HeldItem = nullptr;
	AddReplicatedSubObject(HeldItem);
}


//Delegates
void UHolderComponent::OnOwnerPawnControllerChanged(APawn* InPawn, AController* OldController, AController* NewController)
{
	if (LookTraceSubsystem) return;
	
	TryInitTraceSubsystem();
}