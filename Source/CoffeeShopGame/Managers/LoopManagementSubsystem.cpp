// Fill out your copyright notice in the Description page of Project Settings.


#include "LoopManagementSubsystem.h"
#include "Systems/TicketSystem/TicketManager.h"


void ULoopManagementSubsystem::SetTicketManagerReference(ATicketManager* TicketManager)
{
	if (!IsValid(TicketManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TicketManager reference passed to LoopManagementSubsystem"));
		return;
	}
	
	TicketManagerReference = TicketManager;
}

ATicketManager* ULoopManagementSubsystem::GetTicketManagerReference() const
{
	if (!IsValid(TicketManagerReference))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ticket Manager Reference in LoopManagementSubsystem is invalid"));
		return nullptr;
	}
	return TicketManagerReference;
}

void ULoopManagementSubsystem::SetQueueReference(AQueue* _queue)
{
	if (IsValid(_queue))
		Queue = _queue;
}

AQueue* ULoopManagementSubsystem::GetQueueReference() const
{
	return Queue.Get();
}

void ULoopManagementSubsystem::SetBell(AActor* bellActor)
{
	if (IsValid(bellActor))
		Bell = bellActor;
}

AActor* ULoopManagementSubsystem::GetBellActor() const
{
	return Bell;
}
