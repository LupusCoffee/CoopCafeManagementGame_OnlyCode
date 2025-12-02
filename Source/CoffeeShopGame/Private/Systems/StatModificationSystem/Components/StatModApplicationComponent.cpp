#include "Systems/StatModificationSystem/Components/StatModApplicationComponent.h"
#include "Components/SphereComponent.h"
#include "EngineUtils.h"


UStatModApplicationComponent::UStatModApplicationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatModApplicationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CreateSphereCollider();
	
	RegisterStatHandlersAndInitialOverlapCheck();
	
	SubscribeToOverlapDelegates();
}

void UStatModApplicationComponent::CreateSphereCollider()
{
	SphereCollider = NewObject<USphereComponent>(GetOwner());
	
	SphereCollider->AttachToComponent(GetOwner()->GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	SphereCollider->SetRelativeLocation(FVector::ZeroVector);
	
	SphereCollider->InitSphereRadius(ApplicationRadius);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // example
	
	SphereCollider->RegisterComponent();
}

void UStatModApplicationComponent::RegisterStatHandlersAndInitialOverlapCheck()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		UStatHandlerComponent* StatHandler = Actor->GetComponentByClass<UStatHandlerComponent>();
		if (StatHandler)
		{
			RegisteredStatHandlers.Add(StatHandler);
			
			//initial overlap check
			float RadiusFromSphereCenter = (SphereCollider->GetComponentLocation() - StatHandler->GetOwner()->GetActorLocation()).Size();
			if (RadiusFromSphereCenter < ApplicationRadius) OnStatHandlerEnterRadius(StatHandler);
		}
	}
}

void UStatModApplicationComponent::SubscribeToOverlapDelegates()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &UStatModApplicationComponent::OnActorEnterRadius);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &UStatModApplicationComponent::OnActorExitRadius);
}


void UStatModApplicationComponent::SetStatImpulse(UStatImpulse* StatImpulse)
{
	CurrentStatImpulse = StatImpulse;
}

void UStatModApplicationComponent::SetStatMod(UStatMod* StatMod)
{
	PreviousStatMod = CurrentStatMod;
	CurrentStatMod = StatMod;
}

void UStatModApplicationComponent::OnActorEnterRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UStatHandlerComponent* StatHandler = OtherActor->FindComponentByClass<UStatHandlerComponent>();
	if (!StatHandler) return;
	
	OnStatHandlerEnterRadius(StatHandler);
}

void UStatModApplicationComponent::OnActorExitRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UStatHandlerComponent* StatHandler = OtherActor->FindComponentByClass<UStatHandlerComponent>();
	if (!StatHandler) return;

	OnStatHandlerExitRadius(StatHandler);
}

void UStatModApplicationComponent::OnStatHandlerEnterRadius(UStatHandlerComponent* StatHandlerComponent)
{
	StatHandlersInRange.Add(StatHandlerComponent);
	
	if (!bModShouldBeApplied) return;

	AddCurrentStatModToStatHandler(StatHandlerComponent);
}

void UStatModApplicationComponent::OnStatHandlerExitRadius(UStatHandlerComponent* StatHandlerComponent)
{
	StatHandlersInRange.Remove(StatHandlerComponent);
	
	if (!bModShouldBeApplied) return;

	RemoveCurrentStatModFromStatHandler(StatHandlerComponent);
}


void UStatModApplicationComponent::UpdateStatHandlers(bool bApplyStatMod)
{
	bModShouldBeApplied = bApplyStatMod;
	
	if (bModShouldBeApplied)
	{
		for (auto StatHandlerInRange : StatHandlersInRange)
		{
			if (CurrentStatImpulse) StatHandlerInRange->AddStatImpulse(CurrentStatImpulse);	//need to do a timer on this probably --> talk to mr brajja later
			AddCurrentStatModToStatHandler(StatHandlerInRange);
		}
	}
	else
	{
		for (auto StatHandlerInRange : StatHandlersInRange)
		{
			RemoveCurrentStatModFromStatHandler(StatHandlerInRange);
		}
	}
}


void UStatModApplicationComponent::AddCurrentStatModToStatHandler(UStatHandlerComponent* StatHandlerComponent)
{
	if (!CurrentStatMod) return;
	if (AppliedStatHandlers.Contains(StatHandlerComponent)) return;

	StatHandlerComponent->AddStatMod(CurrentStatMod);
	AppliedStatHandlers.Add(StatHandlerComponent);
}

void UStatModApplicationComponent::RemoveCurrentStatModFromStatHandler(UStatHandlerComponent* StatHandlerComponent)
{
	if (!AppliedStatHandlers.Contains(StatHandlerComponent)) return;
	
	if (CurrentStatMod) StatHandlerComponent->RemoveStatMod(CurrentStatMod);
	else if (PreviousStatMod) StatHandlerComponent->RemoveStatMod(PreviousStatMod);
	else return;

	AppliedStatHandlers.Remove(StatHandlerComponent);
}
