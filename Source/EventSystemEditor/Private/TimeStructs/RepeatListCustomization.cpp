#include "RepeatListCustomization.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "EventSystemEditorPublic.h"
#include "ZeroPaddedIntInterface.h"
#include "EventConstants.h"

#define LOCTEXT_NAMESPACE "TimeCustomization"

TSharedRef<IPropertyTypeCustomization> FEventSystemEditorPublic::MakeRepeatListCustomizationWidget()
{
	return MakeShareable(new FRepeatListCustomization());
}

FRepeatListCustomization::FRepeatListCustomization()
{
	SelectedButtonStyle = FButtonStyle()
		.SetNormal(FSlateRoundedBoxBrush(FLinearColor(0, 0, 0.416), 4.0f, FStyleColors::Input, 1))
		.SetHovered(FSlateRoundedBoxBrush(FLinearColor(0, 0, 0.917), 4.0f, FStyleColors::Input, 1))
		.SetPressed(FSlateRoundedBoxBrush(FLinearColor(0, 0, 0.238), 4.0f, FStyleColors::Input, 1))
		.SetDisabled(FSlateRoundedBoxBrush(FLinearColor(0, 0, 0.0385), 4.0f, FStyleColors::Recessed, 1))
		.SetNormalForeground(FStyleColors::ForegroundHover)
		.SetHoveredForeground(FStyleColors::ForegroundHover)
		.SetPressedForeground(FStyleColors::ForegroundHover)
		.SetDisabledForeground(FStyleColors::Foreground)
		.SetNormalPadding(FMargin(12, 1.5))
		.SetPressedPadding(FMargin(12, 2.5, 12, 0.5));
		
}

void FRepeatListCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> SecondHandle = InStructPropertyHandle->GetChildHandle(FName("RepeatingDays"));
	ArrayHandle = SecondHandle->AsArray();

	HeaderRow
		.NameContent()
		[
			InStructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(500.0f)
		[
			SNew(SBorder)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Fill)
						[
							GenerateOption(0)
						]
					+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Fill)
						[
							GenerateOption(1)
						]
					 + SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Fill)
						[
							GenerateOption(2)
						]
					+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Fill)
						[
							GenerateOption(3)
						]
					+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Fill)
						[
							GenerateOption(4)
						]
					+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Fill)
						[
							GenerateOption(5)
						]
					+ SHorizontalBox::Slot()
						.FillWidth(1)
						.HAlign(HAlign_Fill)
						[
							GenerateOption(6)
						]
				]
		]; 
}

TSharedRef<SWidget> FRepeatListCustomization::GenerateOption(int Day)
{
	TSharedPtr<SButton> NewToggle;

	TSharedPtr<IPropertyHandle> ElementHandle = ArrayHandle->GetElement(Day);
	if(!ElementHandle.IsValid())
		return SNew(STextBlock)
				.Text(FText::FromString(TEXT("Invalid Element Handle")));

	bool Value;
	ElementHandle->GetValue(Value);

	SAssignNew(NewToggle, SButton)
		.ButtonStyle(Value ? &SelectedButtonStyle : &FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
				.Text(FText::FromString(WeekDayStrings[Day]))
		];

	NewToggle->SetOnClicked(FOnClicked::CreateLambda(
		[NewToggle, this, Day]()
		{
			TSharedPtr<IPropertyHandle> ElementHandle = ArrayHandle->GetElement(Day);

			if (!ElementHandle.IsValid())
				return FReply::Handled();

			bool Value;
			ElementHandle->GetValue(Value);

			Value = !Value;
			ElementHandle->SetValue(Value);

			NewToggle->SetButtonStyle(Value ? &SelectedButtonStyle : &FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button"));

			return FReply::Handled();
		}
	));

	return NewToggle.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE