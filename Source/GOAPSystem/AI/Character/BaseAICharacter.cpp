// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"

#include "GOAPSystem/AI/Events/AILifecycleEvents.h"


// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ActivatableComp = CreateDefaultSubobject<UActivatableComponent>("ActivatableComponent");
	GoapComp = CreateDefaultSubobject<UGoapComponent>("GOAP");
	DebugComp = CreateDefaultSubobject<UAIDebugComponent>("DebugComponent");
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Broadcast spawn event - no direct dependency on AIManager!
	if (bAutoRegister)
	{
		FString Name = DebugName.IsEmpty() ? GetName() : DebugName;
		UAILifecycleEvents::BroadcastAISpawned(GetWorld(), this, Name);
	}
}

void ABaseAICharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	// World teardown can invalidate delegate state; AIManager handles full cleanup in subsystem deinit.
	if (World && !World->bIsTearingDown && World->IsGameWorld())
	{
		UAILifecycleEvents::BroadcastAIDestroyed(World, this);
	}
    
	Super::EndPlay(EndPlayReason);
}

void ABaseAICharacter::OnPoolSpawned_Implementation(FVector Location, FRotator Rotation)
{
	
}

void ABaseAICharacter::OnPoolReturned_Implementation()
{
	if (GoapComp)
	{
		GoapComp->ResetExecutionState();
	}
}
