#pragma once

#include "IPropertyTypeCustomization.h"

struct EVisibility;

class IPropertyHandle;
class FDetailWidgetRow;
class IDetailChildrenBuilder;
class SGameplayTagPicker;

/** Customization for the gameplay tag struct */
class FDateCustomization : public IPropertyTypeCustomization
{
public:

	FDateCustomization();

	/** Overridden to show an edit button to launch the gameplay tag editor */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	/** Overridden to do nothing */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {}

private:

private:

	/** Cached property handle */
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
};