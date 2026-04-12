#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "TimeStructs.h"

struct FEventEntry;
struct FEventData;
class UEventCollection;

class SDayEventList : public SCompoundWidget
{
	DECLARE_DELEGATE_TwoParams(FOnSelectionChangedSignature, TSharedPtr<FEventEntry>, SDayEventList*)
	DECLARE_DELEGATE_TwoParams(FOnCollectionSizeChangedSignature, FDate, FName)

public:
	SLATE_BEGIN_ARGS(SDayEventList)
		: _EventCollection(nullptr)
		{
		}
		SLATE_ARGUMENT(FDate, Date)
		SLATE_ARGUMENT(TArray<TSharedPtr<FEventEntry>>, EventList)
		SLATE_ARGUMENT(UEventCollection*, EventCollection)
		SLATE_EVENT(FOnSelectionChangedSignature, OnSelectionChangedDelegate);
		SLATE_EVENT(FOnCollectionSizeChangedSignature, OnCollectionSizeChangedDelegate);
	SLATE_END_ARGS();

	SDayEventList();

	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

	// Modify UI only, used for when date is modified
	void AddItem(FEventData* EventData);
	void RemoveItem(FEventData* EventData);
	void ClearSelection();

	// Used after collection is modified
	void ClearItems();
	void SetItems(TArray<TSharedPtr<FEventEntry>> NewItems);
	int GetNumItems() { return Items.Num(); }

	FDate GetDate() const { return Date; }

	// Modify UI only, called when event tag is modified, so we don't have to refresh whole list
	void RefreshItemLabels();
private:
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FEventEntry> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnSelectionChanged(TSharedPtr<FEventEntry> Item, ESelectInfo::Type SelectionType);
	TSharedPtr<SWidget> OnContextMenuOpening();
	void AddNewEvent();
	void RemoveEvent();
	bool CanRemoveEvent() const;

private:
	FSlateBrush Brush;
	FDate Date;
	TArray<TSharedPtr<FEventEntry>> Items;
	TSharedPtr<SListView<TSharedPtr<FEventEntry>>> ListView;
	bool NeedReselection = false;
	TSharedPtr<FEventEntry> NewSelection;
	UEventCollection* EventCollection;
	TArray<TSharedPtr<STextBlock>> Labels;

	FOnSelectionChangedSignature OnSelectionChangedDelegate;
	FOnCollectionSizeChangedSignature OnCollectionSizeChangedDelegate;
};