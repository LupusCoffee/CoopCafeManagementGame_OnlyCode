#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "TimeStructs.h"

struct FDate;
class SUniformGridPanel;
class SEventPropertyPanel;
class UEventCollection;
struct FEventEntry;
struct FEventData;
class UDetailPanelDataObject;
class SDayEventList;

class SEventCollectionEditor : public SCompoundWidget, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SEventCollectionEditor)
		{
		}
	SLATE_END_ARGS();

	SEventCollectionEditor();
	~SEventCollectionEditor();

	void Construct(const FArguments& InArgs);
	TSharedRef<SWidget> BuildDetailPanel();
private:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override { return TEXT("SEventCollectionEditor"); }

	FReply IncreaseMonth();
	FReply DecreaseMonth();
	void CreateMonthGrid();
	void RefreshDetailPanel();
	void RefreshAllEventLists();
	void ClearDetailPanel();
	void OnCollectionSorted();
	void OnSelectionChanged(TSharedPtr<FEventEntry> NewSelection, SDayEventList* List);
	void OnSelectionChanged(TSharedPtr<FEventEntry> NewSelection, ESelectInfo::Type SelectionType);
	FReply OnSaveClicked();
	void OnTextCommited(const FText& InText, const ETextCommit::Type InTextAction);
	void OnDateFieldFocused(const FFocusEvent& InFocusEvent, const FWeakWidgetPath& InOldWidgetPath, const TSharedPtr<SWidget>& InOldWidget, const FWidgetPath& InNewWidgetPath, const TSharedPtr<SWidget>& InNewWidget);
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FEventEntry> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnCollectionSizeChanged(FDate ModifiedDate, FName Event);
	TSharedPtr<SWidget> OnContextMenuOpening();
	void AddNewEvent();
	void RemoveEvent();
	bool CanRemoveEvent();
	void UpdateDataList();

private:
	TObjectPtr<UEventCollection> SelectedCollection = nullptr;

	TSharedPtr<SEditableText> DateText;

	TSharedPtr<SUniformGridPanel> WrapPanel;
	TArray<TWeakPtr<SDayEventList>> DayEventLists;
	TArray<TWeakPtr<SDayEventList>> DaysWithEvents;

	SDayEventList* ListWithSelection = nullptr;
	FEventData* EditingEvent = nullptr;

	TSharedPtr<IDetailsView> DetailViewWidget;
	TObjectPtr<UDetailPanelDataObject> ViewedObject;

	TSharedPtr<SListView<TSharedPtr<FEventEntry>>> AllEventListWidget;

	FDate CurrentDate;
	FTimeFrame TimeFrame;

	FDelegateHandle SelectionDelegateHandle;
	FDelegateHandle FocusDelegateHandle;

	TArray<TSharedPtr<FEventEntry>> ListItems;

	TObjectPtr<UBlueprint> AnnouncerBlueprint = nullptr;

	bool SavedFromEditor = false;
};