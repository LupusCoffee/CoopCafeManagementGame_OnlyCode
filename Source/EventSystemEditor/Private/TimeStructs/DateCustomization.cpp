#include "DateCustomization.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "EventSystemEditorPublic.h"
#include "ZeroPaddedIntInterface.h"

#define LOCTEXT_NAMESPACE "DateCustomization"

TSharedRef<IPropertyTypeCustomization> FEventSystemEditorPublic::MakeDateCustomizationWidget()
{
	return MakeShareable(new FDateCustomization());
}

FDateCustomization::FDateCustomization()
{
}

void FDateCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;

	TSharedPtr<IPropertyHandle> YearHandle = StructPropertyHandle->GetChildHandle(FName("Year"));
	TSharedPtr<IPropertyHandle> MonthHandle = StructPropertyHandle->GetChildHandle(FName("Month"));
	TSharedPtr<IPropertyHandle> DayHandle = StructPropertyHandle->GetChildHandle(FName("Day"));

	TSharedPtr<FZeroPaddedIntInterface> TwoDigitFormatter = MakeShareable(new FZeroPaddedIntInterface(2));
	TSharedPtr<FZeroPaddedIntInterface> FourDigitFormatter = MakeShareable(new FZeroPaddedIntInterface(4));

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
									.TypeInterface(FourDigitFormatter)
									.MinValue(1)
									.Value_Lambda(
										[YearHandle]() -> TOptional<int32>
										{
											int32 Value;
											if (YearHandle.IsValid() && YearHandle->GetValue(Value) == FPropertyAccess::Success) return Value;
											return TOptional<int32>();
										}
									)
									.OnValueCommitted_Lambda(
										[YearHandle](int32 NewValue, ETextCommit::Type CommitType) 
										{
											if (YearHandle.IsValid()) 
												YearHandle->SetValue(NewValue);
										}
									)						
							]
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
									.Text(FText::FromString(TEXT("/")))
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
										[MonthHandle]() -> TOptional<int32>
										{
											int32 Value;
											if (MonthHandle.IsValid() && MonthHandle->GetValue(Value) == FPropertyAccess::Success) return Value;
											return TOptional<int32>();
										}
									)
									.OnValueCommitted_Lambda(
										[MonthHandle](int32 NewValue, ETextCommit::Type CommitType)
										{
											if (MonthHandle.IsValid())
												MonthHandle->SetValue(NewValue);
										}
									)
							]
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
									.Text(FText::FromString(TEXT("/")))
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
										[DayHandle]() -> TOptional<int32>
										{
											int32 Value;
											if (DayHandle.IsValid() && DayHandle->GetValue(Value) == FPropertyAccess::Success) return Value;
											return TOptional<int32>();
										}
									)
									.OnValueCommitted_Lambda(
										[DayHandle](int32 NewValue, ETextCommit::Type CommitType)
										{
											if (DayHandle.IsValid())
												DayHandle->SetValue(NewValue);
										}
									)
							]
				]
		];
}

#undef LOCTEXT_NAMESPACE