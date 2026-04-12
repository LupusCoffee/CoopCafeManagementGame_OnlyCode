#include "TimeCustomization.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "EventSystemEditorPublic.h"
#include "ZeroPaddedIntInterface.h"

#define LOCTEXT_NAMESPACE "TimeCustomization"

TSharedRef<IPropertyTypeCustomization> FEventSystemEditorPublic::MakeTimeCustomizationWidget()
{
	return MakeShareable(new FTimeCustomization());
}

FTimeCustomization::FTimeCustomization()
{
}

void FTimeCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;

	TSharedPtr<IPropertyHandle> HourHandle = StructPropertyHandle->GetChildHandle(FName("Hour"));
	TSharedPtr<IPropertyHandle> MinuteHandle = StructPropertyHandle->GetChildHandle(FName("Minute"));
	TSharedPtr<IPropertyHandle> SecondHandle = StructPropertyHandle->GetChildHandle(FName("Second"));

	TSharedPtr<FZeroPaddedIntInterface> TwoDigitFormatter = MakeShareable(new FZeroPaddedIntInterface(2));

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SBox)
				.Padding(FMargin(0, 2, 0, 1))
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SNumericEntryBox<int32>)
								.TypeInterface(TwoDigitFormatter)
								.MinValue(1)
								.Value_Lambda(
									[HourHandle]() -> TOptional<int32>
									{
										int32 Value;
										if (HourHandle.IsValid() && HourHandle->GetValue(Value) == FPropertyAccess::Success) return Value;
										return TOptional<int32>();
									}
								)
								.OnValueCommitted_Lambda(
									[HourHandle](int32 NewValue, ETextCommit::Type CommitType)
									{
										if (HourHandle.IsValid())
											HourHandle->SetValue(NewValue);
									}
								)
						]
					+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Top)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT(":")))
								.Font(FStyleDefaults::GetFontInfo(14))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SNumericEntryBox<int32>)
								.TypeInterface(TwoDigitFormatter)
								.MinValue(1)
								.MaxValue(12)
								.Value_Lambda(
									[MinuteHandle]() -> TOptional<int32>
									{
										int32 Value;
										if (MinuteHandle.IsValid() && MinuteHandle->GetValue(Value) == FPropertyAccess::Success) return Value;
										return TOptional<int32>();
									}
								)
								.OnValueCommitted_Lambda(
									[MinuteHandle](int32 NewValue, ETextCommit::Type CommitType)
									{
										if (MinuteHandle.IsValid())
											MinuteHandle->SetValue(NewValue);
									}
								)
						]
					+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Top)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT(":")))
								.Font(FStyleDefaults::GetFontInfo(14))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SNumericEntryBox<int32>)
								.TypeInterface(TwoDigitFormatter)
								.MinValue(1)
								.MaxValue(31)
								.Value_Lambda(
									[SecondHandle]() -> TOptional<int32>
									{
										int32 Value;
										if (SecondHandle.IsValid() && SecondHandle->GetValue(Value) == FPropertyAccess::Success) return Value;
										return TOptional<int32>();
									}
								)
								.OnValueCommitted_Lambda(
									[SecondHandle](int32 NewValue, ETextCommit::Type CommitType)
									{
										if (SecondHandle.IsValid())
											SecondHandle->SetValue(NewValue);
									}
								)
						]
				]
		];
}

#undef LOCTEXT_NAMESPACE