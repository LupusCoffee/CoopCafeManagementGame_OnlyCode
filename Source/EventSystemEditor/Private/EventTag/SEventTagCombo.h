#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "SEventTagPicker.h"

class SEventTagCombo : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEventTagCombo)
		: _PropertyHandle(nullptr)
		{
		}
		SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)
		SLATE_ARGUMENT(FString, TagName)
		SLATE_ARGUMENT(ETagType, TagType)
		SLATE_EVENT(FOnTagChangedSignature, OnTagChangedDelegate)
	SLATE_END_ARGS();

	SEventTagCombo();

	void Construct(const FArguments& InArgs);

private:
	bool IsValueEnabled() const
	{
		return true;
	} 

	void OnMenuOpenChanged(const bool bOpen) const;
	TSharedRef<SWidget> OnGetMenuContent();
	void OnTagChanged(const FString& InTag);

private:
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	FString Tag;
	TSharedPtr<SEventTagPicker> TagPicker;
	TSharedPtr<STextBlock> TagTextBlock;
	FOnTagChangedSignature OnTagChangedDelegate;
	ETagType TagType;
};