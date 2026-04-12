#include "SEventTagGraphPin.h"
#include "SEventTagCombo.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "EventTagGraphPin"

void SEventTagGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	TagType = InArgs._TagType;

	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

void SEventTagGraphPin::ParseDefaultValueData()
{
	FString FormattedString = GraphPinObj->GetDefaultAsString();

	TArray<FString> SplitString;

	FormattedString.ParseIntoArray(SplitString, TEXT("\""));

	if (SplitString.Num() < 3)
		TagName = "";
	else
		TagName = SplitString[1];
}

TSharedRef<SWidget> SEventTagGraphPin::GetDefaultValueWidget()
{
	if (GraphPinObj == nullptr)
	{
		return SNullWidget::NullWidget;
	}

	ParseDefaultValueData();

	return SNew(SEventTagCombo)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		.TagName(TagName)
		.TagType(TagType)
		.OnTagChangedDelegate(this, &SEventTagGraphPin::OnTagChanged);
}

void SEventTagGraphPin::OnTagChanged(const FString& NewTag)
{
	TagName = NewTag;

	FString FormattedString = TEXT("(TagName=\"");
	FormattedString += TagName;
	FormattedString += TEXT("\")");

	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();

	if (!CurrentDefaultValue.Equals(FormattedString))
	{
		const FScopedTransaction Transaction(LOCTEXT("ChangeDefaultValue", "Change Pin Default Value"));
		GraphPinObj->Modify();
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, FormattedString);
	}
}

#undef LOCTEXT_NAMESPACE
