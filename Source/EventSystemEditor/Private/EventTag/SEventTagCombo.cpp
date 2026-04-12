#include "SEventTagCombo.h"
#include "Widgets/Input/SComboButton.h"

SEventTagCombo::SEventTagCombo()
{

}

void SEventTagCombo::Construct(const FArguments& InArgs)
{
	PropertyHandle = InArgs._PropertyHandle;
	OnTagChangedDelegate = InArgs._OnTagChangedDelegate;
	TagType = InArgs._TagType;

	TArray<FString> SplitString;
	FString TagName;

	if (PropertyHandle.IsValid())
	{
		FString ValueString;
		PropertyHandle->GetValueAsFormattedString(ValueString);

		ValueString.ParseIntoArray(SplitString, TEXT("\""));

		if (SplitString.Num() < 3)
			TagName = "None";
		else
			TagName = SplitString[1];
	}
	else
	{
		Tag = InArgs._TagName.IsEmpty() ? "None" : InArgs._TagName;
		TagName = Tag;
	}

	ChildSlot
	[
		SNew(SHorizontalBox) // Extra box to make the combo hug the chip

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ComboButton, SComboButton)
					.ComboButtonStyle(FAppStyle::Get(), "ComboButton")
					.HasDownArrow(true)
					.ContentPadding(1)
					.IsEnabled(this, &SEventTagCombo::IsValueEnabled)
					.Clipping(EWidgetClipping::OnDemand)
					.OnMenuOpenChanged(this, &SEventTagCombo::OnMenuOpenChanged)
					.OnGetMenuContent(this, &SEventTagCombo::OnGetMenuContent)
					.ButtonContent()
					[
						SAssignNew(TagTextBlock, STextBlock)
							.Text(FText::FromString(TagName))
					]
			]
	];
}

void SEventTagCombo::OnMenuOpenChanged(const bool bOpen) const
{
	if (bOpen && TagPicker.IsValid())
	{
		ComboButton->SetMenuContentWidgetToFocus(TagPicker->GetWidgetToFocusOnOpen());
	}
}

TSharedRef<SWidget> SEventTagCombo::OnGetMenuContent()
{
	TagPicker = SNew(SEventTagPicker)
		.PropertyHandle(PropertyHandle)
		.TagName(Tag)
		.TagType(TagType)
		.MaxHeight(350.0f)
		.OnTagChangedDelegate(this, &SEventTagCombo::OnTagChanged);

	if (TagPicker->GetWidgetToFocusOnOpen())
	{
		ComboButton->SetMenuContentWidgetToFocus(TagPicker->GetWidgetToFocusOnOpen());
	}

	return TagPicker.ToSharedRef();
}

void SEventTagCombo::OnTagChanged(const FString& InTag)
{
	if(InTag.IsEmpty())
		TagTextBlock->SetText(FText::FromString("None"));
	else
		TagTextBlock->SetText(FText::FromString(InTag));

	Tag = InTag;

	OnTagChangedDelegate.ExecuteIfBound(InTag);
}
