#pragma once

#include "EdGraphUtilities.h"
#include "EdGraphSchema_K2.h"

class FEventTagsGraphPanelPinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};