#include "SEventPropertyPanel.h"
#include "EventTag/SEventTagCombo.h"

const FMargin PropertyRowPadding(10, 4);

SEventPropertyPanel::SEventPropertyPanel()
{
}

void SEventPropertyPanel::Construct(const FArguments& InArgs)
{
	EditingEvent = InArgs._EditingEvent;
	SelectedCollection = InArgs._SelectedCollection;

	ChildSlot
		[
			SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.Padding(0)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot() // Event Name
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(PropertyRowPadding)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Details")))
						]
						+ SVerticalBox::Slot() // Event Name
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(PropertyRowPadding)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Event Property Panel")))
						]
						+ SVerticalBox::Slot() // Trigger Date
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(PropertyRowPadding)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Event Property Panel")))
						]
						+ SVerticalBox::Slot() // Trigger Time
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(PropertyRowPadding)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Event Property Panel")))
						]
						+ SVerticalBox::Slot() // Repeating days
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(PropertyRowPadding)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Event Property Panel")))
						]
						+ SVerticalBox::Slot() // Extra Data
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(PropertyRowPadding)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Event Property Panel")))
						]
				]
		];
}
