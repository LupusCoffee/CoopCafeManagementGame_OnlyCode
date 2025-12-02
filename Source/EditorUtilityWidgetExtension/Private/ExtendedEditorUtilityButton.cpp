// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendedEditorUtilityButton.h"

void UExtendedEditorUtilityButton::Init()
{
	OnClicked.AddDynamic(this, &UExtendedEditorUtilityButton::BroadcastOnSelfClicked);
}

void UExtendedEditorUtilityButton::BroadcastOnSelfClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Called"));
	OnSelfClicked.Broadcast(this);
}
