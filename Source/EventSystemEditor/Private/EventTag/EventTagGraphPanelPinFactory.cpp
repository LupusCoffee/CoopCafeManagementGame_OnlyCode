#include "EventTagGraphPanelPinFactory.h"
#include "EventTag/SEventTagGraphPin.h"
#include "EventSystemStructs.h"

TSharedPtr<class SGraphPin> FEventTagsGraphPanelPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		if (UScriptStruct* PinStructType = Cast<UScriptStruct>(InPin->PinType.PinSubCategoryObject.Get()))
		{
			if (PinStructType->IsChildOf(FEventTag::StaticStruct()))
			{
				return SNew(SEventTagGraphPin, InPin)
					.TagType(ETagType::Event);
			}
			else if (PinStructType->IsChildOf(FDataTag::StaticStruct()))
			{
				return SNew(SEventTagGraphPin, InPin)
					.TagType(ETagType::Data);
			}
		}
	}

	return nullptr;
}
