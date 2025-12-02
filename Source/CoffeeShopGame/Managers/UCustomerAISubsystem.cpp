// Fill out your copyright notice in the Description page of Project Settings.


#include "UCustomerAISubsystem.h"

UCustomerAISubsystem::UCustomerAISubsystem(): CustomerManager(nullptr)
{
}

void UCustomerAISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCustomerAISubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UCustomerAISubsystem::AssignCustomerManager(ACustomerManager* _customerManager)
{
	CustomerManager = _customerManager;

	if (IsValid(CustomerManager))
		return true;

	return false;
}

ACustomerManager* UCustomerAISubsystem::GetCustomerManager()
{
	if (IsValid(CustomerManager))
		return CustomerManager;

	return nullptr;
}
