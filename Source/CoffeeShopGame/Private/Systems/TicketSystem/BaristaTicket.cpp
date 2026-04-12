// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/TicketSystem/BaristaTicket.h"

// Sets default values
ABaristaTicket::ABaristaTicket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaristaTicket::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaristaTicket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

