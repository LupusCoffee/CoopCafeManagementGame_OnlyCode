// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendedEditorUtilityWidget.h"
#include "Selection.h"

void UExtendedEditorUtilityWidget::Init()
{
	USelection::SelectionChangedEvent.AddUObject(this, &UExtendedEditorUtilityWidget::BroadcastSelectionChanged);
}

void UExtendedEditorUtilityWidget::BroadcastSelectionChanged(UObject* NewSelection)
{
	OnSelectionChange.Broadcast(NewSelection);
}
