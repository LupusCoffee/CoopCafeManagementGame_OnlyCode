// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

struct EVisibility;

class IPropertyHandle;
class FDetailWidgetRow;
class IDetailChildrenBuilder;
class SGameplayTagPicker;

/** Customization for the gameplay tag struct */
class FEventTagCustomization : public IPropertyTypeCustomization
{
public:

	FEventTagCustomization();

	/** Overridden to show an edit button to launch the gameplay tag editor */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
	/** Overridden to do nothing */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {}

private:

	/** Cached property handle */
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

	// Not sure where is this used
	//FGameplayTag Tag;
};
