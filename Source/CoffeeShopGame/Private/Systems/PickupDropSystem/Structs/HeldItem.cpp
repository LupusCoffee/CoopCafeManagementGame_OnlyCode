#include "Systems/PickupDropSystem/Structs/HeldItem.h"
#include "Net/UnrealNetwork.h"

void UHeldItem::Init(AActor* _Actor, UStaticMeshComponent* _MeshComp, UPrimitiveComponent* _PrimitiveComponent)
{
	Actor = _Actor;
	MeshComp = _MeshComp;
	PrimitiveComponent = _PrimitiveComponent;

	if (MeshComp) InitialCollisionResponses = MeshComp->GetCollisionResponseToChannels();
	if (PrimitiveComponent)	bInitiallyHadPhysics = PrimitiveComponent->IsSimulatingPhysics();
}


//Getters
AActor* UHeldItem::GetActor()
{
	return Actor;
}

UStaticMeshComponent* UHeldItem::GetMeshComp()
{
	return MeshComp;
}

UPrimitiveComponent* UHeldItem::GetPrimitiveComponent()
{
	return PrimitiveComponent;
}

FCollisionResponseContainer UHeldItem::GetInitialCollisionResponses()
{
	return InitialCollisionResponses;
}

bool UHeldItem::InitiallyHadPhysics()
{
	return bInitiallyHadPhysics;
}


//Replication
void UHeldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	//adding any properties that should be replicated
	DOREPLIFETIME(UHeldItem, Actor);
	DOREPLIFETIME(UHeldItem, MeshComp);
	DOREPLIFETIME(UHeldItem, PrimitiveComponent);
	DOREPLIFETIME(UHeldItem, InitialCollisionResponses);
	DOREPLIFETIME(UHeldItem, bInitiallyHadPhysics);
}

bool UHeldItem::IsSupportedForNetworking() const
{
	return true;
}
