#pragma once

#include "IPropertyTypeCustomization.h"

class IPropertyHandle;
class FDetailWidgetRow;
class IDetailChildrenBuilder;

/** Customization for the gameplay tag struct */
class FDataContainerCustomization : public IPropertyTypeCustomization
{
public:

	FDataContainerCustomization();

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** Overridden to show an edit button to launch the gameplay tag editor */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	/** Overridden to do nothing */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {}

private:

	/** Cached property handle */
	TSharedPtr<IPropertyHandleArray> ArrayHandle;
};