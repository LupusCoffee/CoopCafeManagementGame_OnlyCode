// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendedEditorUtilityWidget.h"
#include "Selection.h"

bool UExtendedEditorUtilityWidget::Initialize()
{
	USelection::SelectionChangedEvent.AddUObject(this, &UExtendedEditorUtilityWidget::BroadcastSelectionChanged);

	return Super::Initialize();
}

void UExtendedEditorUtilityWidget::BroadcastSelectionChanged(UObject* NewSelection)
{
	OnSelectionChange.Broadcast(NewSelection);
}
