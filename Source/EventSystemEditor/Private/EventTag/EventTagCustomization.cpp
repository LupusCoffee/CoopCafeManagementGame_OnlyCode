// Copyright Epic Games, Inc. All Rights Reserved.

#include "EventTagCustomization.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "GameplayTagsManager.h"
#include "IDetailChildrenBuilder.h"
#include "EventTag/SEventTagCombo.h"
#include "EventSystemStructs.h"
#include "EventSystemEditorPublic.h"

#define LOCTEXT_NAMESPACE "EventTagCustomization"

TSharedRef<IPropertyTypeCustomization> FEventSystemEditorPublic::MakeTagCustomizationWidget()
{
	return MakeShareable(new FEventTagCustomization());
}

FEventTagCustomization::FEventTagCustomization()
{
}

void FEventTagCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;

	ETagType TagType;
	FProperty* Property = StructPropertyHandle->GetProperty();
	if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		UScriptStruct* ScriptStruct = StructProperty->Struct;
		
		if (ScriptStruct == FEventTag::StaticStruct())
			TagType = ETagType::Event;
		else if (ScriptStruct == FDataTag::StaticStruct())
			TagType = ETagType::Data;
	}

	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SBox)
		.Padding(FMargin(0, 2, 0, 1))
		[
			SNew(SEventTagCombo)
				.PropertyHandle(StructPropertyHandle)
				.TagType(TagType)
		]
	];
}

#undef LOCTEXT_NAMESPACE
