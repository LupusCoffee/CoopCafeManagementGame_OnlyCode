#include "SDayEventList.h"
#include "EventEntry.h"
#include "EventCollection.h"
#include "EventSystemEditorPublic.h"
#include "SEventCollectionEditor.h"

#define LOCTEXT_NAMESPACE "SDayEventList"
DEFINE_LOG_CATEGORY(EventSystemEditor)

SDayEventList::SDayEventList()
{
	Brush.ImageType = ESlateBrushImageType::Type::FullColor;
}

void SDayEventList::Construct(const FArguments& InArgs)
{
	Date = InArgs._Date;

	Items = InArgs._EventList;

	OnSelectionChangedDelegate = InArgs._OnSelectionChangedDelegate;

	EventCollection = InArgs._EventCollection;

	OnCollectionSizeChangedDelegate = InArgs._OnCollectionSizeChangedDelegate;

	ChildSlot
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)
				[
					SNew(SBorder)
						.BorderImage(&Brush)
						.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f))
						.ColorAndOpacity(FLinearColor::White)
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
								.Text(FText::FromString(FString::Printf(TEXT("%d"), Date.Day)))
						] 
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SNew(SBorder)
						.BorderImage(&Brush)
						.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f))
						.ColorAndOpacity(FLinearColor::White)
						[
							SAssignNew(ListView, SListView<TSharedPtr<FEventEntry>>)
								.ListItemsSource(&Items)
								.SelectionMode(ESelectionMode::Single)
								.OnSelectionChanged(this, &SDayEventList::OnSelectionChanged)
								.OnGenerateRow(this, &SDayEventList::OnGenerateRowForList)
								.OnContextMenuOpening(this, &SDayEventList::OnContextMenuOpening)
						]
				]
		];
}

void SDayEventList::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (NeedReselection && NewSelection)
	{
		ListView->SetSelection(NewSelection);
		NeedReselection = false;
		NewSelection = nullptr;
	}
}

void SDayEventList::AddItem(FEventData* EventData)
{
	TSharedPtr<FEventEntry> NewEntry = MakeShared<FEventEntry>(EventData);
	Items.Add(NewEntry);
	ListView->RequestListRefresh();
	NeedReselection = true;
	NewSelection = NewEntry;
}

void SDayEventList::RemoveItem(FEventData* EventData)
{
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i]->EventData == EventData)
		{
			Items.RemoveAt(i);
			ListView->RequestListRefresh();
			return;
		}
	}
}

void SDayEventList::SetItems(TArray<TSharedPtr<FEventEntry>> NewItems)
{
	Items = NewItems;
	ListView->RequestListRefresh();
}


void SDayEventList::ClearItems()
{
	Items.Empty();
}

void SDayEventList::ClearSelection()
{
	ListView->ClearSelection();
}

void SDayEventList::RefreshItemLabels()
{
	if (Labels.Num() != Items.Num())
	{
		UE_LOG(EventSystemEditor, Error, TEXT("Labels and Items count mismatch!"));
		return;
	}

	for(int i = 0; i < Labels.Num(); i++)
	{
		Labels[i]->SetText(FText::FromName(Items[i].Get()->EventData->Event.GetTagName()));
	}
}

TSharedRef<ITableRow> SDayEventList::OnGenerateRowForList(TSharedPtr<FEventEntry> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedPtr<STextBlock> TextBlock = SNew(STextBlock)
		.Text(FText::FromName(Item.Get()->EventData->Event.GetTagName()));

	Labels.Add(TextBlock);

	return SNew(STableRow< TSharedPtr<FEventEntry>>, OwnerTable)
		.Padding(2.0f)
		[
			TextBlock.ToSharedRef()
		];
}

void SDayEventList::OnSelectionChanged(TSharedPtr<FEventEntry> Item, ESelectInfo::Type SelectionType)
{
	if (!Item.IsValid() || SelectionType == ESelectInfo::Type::Direct)
		return;

	OnSelectionChangedDelegate.ExecuteIfBound(Item, this);
}

TSharedPtr<SWidget> SDayEventList::OnContextMenuOpening()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.BeginSection("EditSection", LOCTEXT("Heading", "Edit Action"));

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Add Event")),
		FText::FromString(TEXT("Add a new event to this day")),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([this]()
			{
				AddNewEvent();
			}))
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Remove Event")),
		FText::FromString(TEXT("Remove selected eventy")),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([this]()
			{
				RemoveEvent();
			}),
			FCanExecuteAction::CreateLambda([this]()
			{
				return CanRemoveEvent();
			})
		)
	);

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void SDayEventList::AddNewEvent()
{
	if (!EventCollection)
		return;

	FEventData NewEventData;
	NewEventData.TriggerTime.Date = Date;

	EventCollection->Events.Add(NewEventData);

	OnCollectionSizeChangedDelegate.ExecuteIfBound(Date, NAME_None);
}

void SDayEventList::RemoveEvent()
{
	if (!EventCollection)
		return;

	TSharedPtr<FEventEntry> SelectedItem = ListView->GetSelectedItems()[0];
	FName EventName = SelectedItem->EventData->Event.GetTagName();
	FDate RemoveDate = SelectedItem->EventData->TriggerTime.Date;

	for (int i = 0; i < EventCollection->Events.Num(); i++)
	{
		if (&EventCollection->Events[i] == SelectedItem->EventData)
		{
			EventCollection->Events.RemoveAt(i);
			break;
		}
	}

	// Notify the parent editor to clear detail panel
	// Also cycle through the collection and repopulate the lists again since address of events will change if the event is located in the middle of the array,
	// TArray seems to condense the array after removing an element, which causes the address of events after the removed one to shift up
	OnCollectionSizeChangedDelegate.ExecuteIfBound(RemoveDate, EventName);
}

bool SDayEventList::CanRemoveEvent() const
{
	return ListView->GetNumItemsSelected() > 0;
}

#undef LOCTEXT_NAMESPACE
