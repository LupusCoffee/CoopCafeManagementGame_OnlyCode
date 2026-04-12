#pragma once

#include "KismetPins/SGraphPinStructInstance.h"
#include "TagType.h"

class SEventTagGraphPin : public SGraphPinStructInstance
{
public:
	SLATE_BEGIN_ARGS(SEventTagGraphPin) {}
		SLATE_ARGUMENT(ETagType, TagType)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	//~ Begin SGraphPinStructInstance Interface
	virtual void ParseDefaultValueData() override;
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	//~ End SGraphPin Interface

	//FGameplayTag GetGameplayTag() const;
	void OnTagChanged(const FString& NewTag);

	FString TagName;
	ETagType TagType;
};