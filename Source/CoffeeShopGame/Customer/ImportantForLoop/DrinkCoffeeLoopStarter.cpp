// Fill out your copyright notice in the Description page of Project Settings.


#include "DrinkCoffeeLoopStarter.h"

#include "CoffeeShopGame/Customer/Customer.h"


void ADrinkCoffeeLoopStarter::Interact_Implementation(AActor* Agent)
{
}

// Sets default values
ADrinkCoffeeLoopStarter::ADrinkCoffeeLoopStarter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Tags.Add(FName("CoffeeLoopStarter"));
}

// Called when the game starts or when spawned
void ADrinkCoffeeLoopStarter::BeginPlay()
{
	Super::BeginPlay();
	
}
