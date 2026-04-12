#pragma once

#include "IPropertyTypeCustomization.h"

struct EVisibility;

class IPropertyHandle;
class FDetailWidgetRow;
class IDetailChildrenBuilder;
class SGameplayTagPicker;

/** Customization for the gameplay tag struct */
class FRepeatListCustomization : public IPropertyTypeCustomization
{
public:

	FRepeatListCustomization();

	/** Overridden to show an edit button to launch the gameplay tag editor */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	/** Overridden to do nothing */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {}

private:
	TSharedRef<SWidget> GenerateOption(int Day);

private:

	/** Cached property handle */
	TSharedPtr<IPropertyHandleArray> ArrayHandle;
	FButtonStyle SelectedButtonStyle;
};