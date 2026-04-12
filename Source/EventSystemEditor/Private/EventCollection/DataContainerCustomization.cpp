#include "DataContainerCustomization.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "EventSystemStructs.h"
#include "EventSystemEditorPublic.h"

#define LOCTEXT_NAMESPACE "DataContainerCustomization"

TSharedRef<IPropertyTypeCustomization> FEventSystemEditorPublic::MakeDataContainerCustomizationWidget()
{
	return MakeShareable(new FDataContainerCustomization());
}

FDataContainerCustomization::FDataContainerCustomization()
{
}

void FDataContainerCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> SecondHandle = InStructPropertyHandle->GetChildHandle(FName("Data"));
	ArrayHandle = SecondHandle->AsArray();

	HeaderRow
		.NameContent()
		[
			InStructPropertyHandle->CreatePropertyNameWidget()
		]
		/*.ValueContent()
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					StructPropertyHandle->CreatePropertyValueWidget()
				]
		]*/;
}

#undef LOCTEXT_NAMESPACE
