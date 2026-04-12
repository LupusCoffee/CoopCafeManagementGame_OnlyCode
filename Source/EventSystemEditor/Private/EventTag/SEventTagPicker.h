#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "TagType.h"

DECLARE_DELEGATE_OneParam(FOnTagChangedSignature, const FString&)

class SEventTagPicker : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SEventTagPicker)
		: _MaxHeight(260.0f)
		, _PropertyHandle(nullptr)
		{
		}
		SLATE_ARGUMENT(float, MaxHeight)
		SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)
		SLATE_ARGUMENT(FString, TagName)
		SLATE_ARGUMENT(ETagType, TagType)
		SLATE_EVENT(FOnTagChangedSignature, OnTagChangedDelegate)
	SLATE_END_ARGS()

	SEventTagPicker();

	void Construct(const FArguments& InArgs);
	TSharedPtr<SWidget> GetWidgetToFocusOnOpen();

private:
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnTagCheckStatusChanged(ECheckBoxState NewCheckState, TSharedPtr<FString> InTag);
	ECheckBoxState IsTagChecked(TSharedPtr<FString> InTag) const;
	FString ParsePropertyHandle(TSharedPtr<IPropertyHandle> InPropertyHandle);

private:
	float MaxHeight = 260.0f;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TArray<TSharedPtr<FString>> Items;
	TSharedPtr<SListView<TSharedPtr<FString>>> ListViewWidget;
	TSharedPtr<FString> SelectedItem;
	FOnTagChangedSignature OnTagChangedDelegate;
};