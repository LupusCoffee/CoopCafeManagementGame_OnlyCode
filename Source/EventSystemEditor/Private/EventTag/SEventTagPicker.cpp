#include "SEventTagPicker.h"
#include "Widgets/Views/SListView.h"
#include "EventTagSettings.h"

#define LOCTEXT_NAMESPACE "EventTagPicker"

SEventTagPicker::SEventTagPicker()
{
	const UEventTagSettings* Settings = GetDefault<UEventTagSettings>();

	for (auto& InTag : Settings->GetEventList())
	{
		Items.Add(MakeShareable(new FString(InTag.Event)));
	}
}

void SEventTagPicker::Construct(const FArguments& InArgs)
{
	MaxHeight = InArgs._MaxHeight;
	OnTagChangedDelegate = InArgs._OnTagChangedDelegate;
	PropertyHandle = InArgs._PropertyHandle;

	Items.Empty();
	const UEventTagSettings* Settings = GetDefault<UEventTagSettings>();
	TArray<FString> TagList;

	if (InArgs._TagType == ETagType::Event)
		TagList = Settings->GetEventTagList();
	else
		TagList = Settings->GetDataTagList();

	for (auto& InTag : TagList)
	{
		Items.Add(MakeShareable(new FString(InTag)));
	}

	FString Value;

	if (PropertyHandle.IsValid())
		Value = ParsePropertyHandle(PropertyHandle);
	else
		Value = InArgs._TagName;

	for (auto& Item : Items)
		if (Item.Get()->Equals(Value))
		{
			SelectedItem = Item;
			break;
		}

	TSharedRef<SWidget> Picker =
		SNew(SBox)
		.WidthOverride(300.0f)
		.HeightOverride(MaxHeight)
		[
			SNew(SBorder)
				.BorderImage(FStyleDefaults::GetNoBrush())
				[
					SAssignNew(ListViewWidget, SListView<TSharedPtr<FString>>)
						.ListItemsSource(&Items)
						.OnGenerateRow(this, &SEventTagPicker::OnGenerateRowForList)
				]
		];

	FMenuBuilder MenuBuilder(/*bInShouldCloseWindowAfterMenuSelection*/false, nullptr);

	MenuBuilder.BeginSection(FName(), LOCTEXT("EventTagList", "EventTag"));

	MenuBuilder.AddWidget(Picker, FText::GetEmpty(), true);

	MenuBuilder.EndSection();


	ChildSlot
	[
		MenuBuilder.MakeWidget()
	];
}

TSharedPtr<SWidget> SEventTagPicker::GetWidgetToFocusOnOpen()
{
	return SharedThis(this);
}

TSharedRef<ITableRow> SEventTagPicker::OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	//Create the row
	return
		SNew(STableRow< TSharedPtr<FString>>, OwnerTable)
		.Padding(2.0f)
		[
			SNew(SCheckBox)
				.OnCheckStateChanged(this, &SEventTagPicker::OnTagCheckStatusChanged, Item)
				.IsChecked(this, &SEventTagPicker::IsTagChecked, Item)
				.CheckBoxContentUsesAutoWidth(false)
				[
					SNew(STextBlock)
						.Text(FText::FromString(*Item.Get()))
				]
		];
}

void SEventTagPicker::OnTagCheckStatusChanged(ECheckBoxState NewCheckState, TSharedPtr<FString> InTag)
{
	FString DataString;

	if (NewCheckState == ECheckBoxState::Checked)
	{
		DataString = *InTag.Get();
		SelectedItem = InTag;
		OnTagChangedDelegate.ExecuteIfBound(*InTag.Get());
	}
	else if (NewCheckState == ECheckBoxState::Unchecked)
	{
		DataString = "";
		SelectedItem = nullptr;
		OnTagChangedDelegate.ExecuteIfBound("");
	}

	if (PropertyHandle.IsValid())
	{
		FString FormattedString = TEXT("(TagName=\"");
		FormattedString += DataString;
		FormattedString += TEXT("\")");
		PropertyHandle->SetValueFromFormattedString(FormattedString);
	}
}

ECheckBoxState SEventTagPicker::IsTagChecked(TSharedPtr<FString> InTag) const
{
	if (InTag == SelectedItem)
		return ECheckBoxState::Checked;
	else
		return ECheckBoxState::Unchecked;
}

FString SEventTagPicker::ParsePropertyHandle(TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	TArray<FString> OutString;

	FString ValueString;
	InPropertyHandle->GetValueAsFormattedString(ValueString);

	ValueString.ParseIntoArray(OutString, TEXT("\""));

	if (OutString.Num() < 3)
		return FString();
	else
		return OutString[1];
}

#undef LOCTEXT_NAMESPACE