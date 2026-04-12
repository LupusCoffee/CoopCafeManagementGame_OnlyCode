#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"

struct FEventData;
class UEventCollection;

class SEventPropertyPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEventPropertyPanel)
		: _EditingEvent(nullptr)
		, _SelectedCollection(nullptr)
		{
		}
		SLATE_ARGUMENT(FEventData*, EditingEvent)
		SLATE_ARGUMENT(UEventCollection*, SelectedCollection)
	SLATE_END_ARGS();

	SEventPropertyPanel();

	void Construct(const FArguments& InArgs);
private:


private:
	FSlateBrush Brush;
	FEventData* EditingEvent;
	UEventCollection* SelectedCollection;
};