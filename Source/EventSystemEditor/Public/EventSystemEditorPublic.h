#pragma once

DECLARE_LOG_CATEGORY_EXTERN(EventSystemEditor, Log, All);

struct EVENTSYSTEMEDITOR_API FEventSystemEditorPublic
{
	static TSharedRef<SWidget> MakeEventCollectionEditorWidget();
	static TSharedRef<IPropertyTypeCustomization> MakeTagCustomizationWidget();
	static TSharedRef<IPropertyTypeCustomization> MakeDateCustomizationWidget();
	static TSharedRef<IPropertyTypeCustomization> MakeTimeCustomizationWidget();
	static TSharedRef<IPropertyTypeCustomization> MakeRepeatListCustomizationWidget();
	static TSharedRef<IPropertyTypeCustomization> MakeDataContainerCustomizationWidget();
};