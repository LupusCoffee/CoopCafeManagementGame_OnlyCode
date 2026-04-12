#include "SEventCollectionEditor.h"
#include "EventSystemEditorPublic.h"
#include "Components/UniformGridSlot.h"
#include "DetailPanelDataObject.h"
#include "DayEnum.h"
#include "SDayEventList.h"
#include "TimeDateUtility.h"
#include "Selection.h"
#include "EventCollection.h"
#include "EventSystemSetting.h"
#include "EventEntry.h"
#include "SEventPropertyPanel.h"
#include "FileHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "EventConstants.h"
#include <EventDelegateUtility.h>
#include "EventTagSettings.h"

#define LOCTEXT_NAMESPACE "EventCollectionEditor"

const UEnum* WeekDayEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDayEnum"), true);

TSharedRef<SWidget> FEventSystemEditorPublic::MakeEventCollectionEditorWidget()
{
	// Put your tab content here!
	return SNew(SEventCollectionEditor);
}

SEventCollectionEditor::SEventCollectionEditor()
{
	CurrentDate.Year = 2026;
	CurrentDate.Month = 2;
	CurrentDate.Day = 17;
	TimeFrame.DateOnly = true;
}

SEventCollectionEditor::~SEventCollectionEditor()
{
	if (FSlateApplication::IsInitialized() && FocusDelegateHandle.IsValid())
	{
		FSlateApplication::Get().OnFocusChanging().Remove(FocusDelegateHandle);
	}
}

void SEventCollectionEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	// Tell the Garbage Collector that we are explicitly referencing these objects!
	if (ViewedObject)
	{
		Collector.AddReferencedObject(ViewedObject);
	}

	if (AnnouncerBlueprint)
	{
		Collector.AddReferencedObject(AnnouncerBlueprint);
	}

	if (SelectedCollection)
	{
		Collector.AddReferencedObject(SelectedCollection);
	}
}

void SEventCollectionEditor::Construct(const FArguments& InArgs)
{
	FString BPPath = TEXT("/Game/EventSystem/BP_EventAnnouncer.BP_EventAnnouncer");

	AnnouncerBlueprint = LoadObject<UBlueprint>(nullptr, *BPPath);

	if (AnnouncerBlueprint == nullptr)
	{
		ChildSlot
			[
				SNew(STextBlock)
					.Text(FText::FromString(TEXT("Failed to load Announcer Blueprint, please make sure the asset exists at " + BPPath)))
			];

		return;
	}

	// Reading from the setting for now, had issue with USelection OnSelectionChanged Event, will look for other solution
	const UEventSystemSetting* Setting = UEventSystemSetting::GetEventSettings();
	if (Setting != nullptr)
	{
		SelectedCollection = Setting->EventCollection.LoadSynchronous();
		SelectedCollection->OnPostUndoDelegate.BindRaw(this, &SEventCollectionEditor::RefreshDetailPanel);
	}

	if (FSlateApplication::IsInitialized())
	{
		FocusDelegateHandle = FSlateApplication::Get().OnFocusChanging().AddSP(this, &SEventCollectionEditor::OnDateFieldFocused);
	}

	ViewedObject = NewObject<UDetailPanelDataObject>();
	ViewedObject->OnDataChanged.BindSP(this, &SEventCollectionEditor::UpdateDataList);

	TSharedPtr<SUniformGridPanel> WeekDayPanel;
	ChildSlot
		.Padding(FMargin(4, 0))
		[
			SNew(SBorder)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FSlateColor(FLinearColor(0.018f, 0.018f, 0.018f)))
				.Padding(0)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(2.0f)
						.HAlign(HAlign_Fill)
						[
							SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								.VAlign(VAlign_Top)
								.HAlign(HAlign_Center)
								[
									SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.HAlign(HAlign_Center)
										[
											SNew(SButton)
												.OnClicked(FOnClicked::CreateRaw(this, &SEventCollectionEditor::DecreaseMonth))
												[
													SNew(STextBlock)
														.Text(FText::FromString(TEXT("<")))
														.Font(FStyleDefaults::GetFontInfo(20))
												]
										]
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.HAlign(HAlign_Center)
										.VAlign(VAlign_Center)
										[
											SNew(SBox)
												.WidthOverride(200)
												.HAlign(HAlign_Center)
												.VAlign(VAlign_Center)
												[
													SAssignNew(DateText, SEditableText)
														.Text(FText::FromString(MonthStrings[CurrentDate.Month - 1] + " " + FString::FromInt(CurrentDate.Year)))
														.Font(FStyleDefaults::GetFontInfo(20))
														.OnTextCommitted(this, &SEventCollectionEditor::OnTextCommited)
												]
										]
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.HAlign(HAlign_Center)
										[
											SNew(SButton)
												.OnClicked(FOnClicked::CreateRaw(this, &SEventCollectionEditor::IncreaseMonth))
												[
													SNew(STextBlock)
														.Text(FText::FromString(TEXT(">")))
														.Font(FStyleDefaults::GetFontInfo(20))
												]
										]
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.VAlign(VAlign_Top)
								[
									SAssignNew(WeekDayPanel, SUniformGridPanel)
										.SlotPadding(FMargin(2))
								]
								+ SVerticalBox::Slot()
								.FillHeight(1.0f)
								.VAlign(VAlign_Fill)
								.HAlign(HAlign_Fill)
								[
									SAssignNew(WrapPanel, SUniformGridPanel)
										.SlotPadding(FMargin(2))
								]	
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SSeparator)
								.Thickness(4)
								.Orientation(EOrientation::Orient_Vertical)
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.HAlign(HAlign_Fill)
						[
							SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.FillHeight(1.0f)
								.VAlign(VAlign_Fill)
								[
									SNew(SVerticalBox)
										+ SVerticalBox::Slot()
										.AutoHeight()
										.Padding(FMargin(10, 5))
										[
											SNew(STextBlock)
												.Text(FText::FromString(TEXT("Event List")))
												.Font(FStyleDefaults::GetFontInfo(16))
										]
										+ SVerticalBox::Slot()
										.FillHeight(1)
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Fill)
										.Padding(FMargin(5))
										[
											SAssignNew(AllEventListWidget, SListView<TSharedPtr<FEventEntry>>)
												.ListItemsSource(&ListItems)
												.OnGenerateRow(this, &SEventCollectionEditor::OnGenerateRowForList)
												.OnSelectionChanged(this, &SEventCollectionEditor::OnSelectionChanged)
												.OnContextMenuOpening(this, &SEventCollectionEditor::OnContextMenuOpening)
												
										]
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(SSeparator)
										.Thickness(4)
								]
								+ SVerticalBox::Slot()
								.FillHeight(1.0f)
								.VAlign(VAlign_Fill)
								.HAlign(HAlign_Fill)
								[
									BuildDetailPanel()
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(SSeparator)
										.Thickness(4)
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.VAlign(VAlign_Bottom)
								.HAlign(HAlign_Fill)
								[
									SNew(SButton)
										.OnClicked(this, &SEventCollectionEditor::OnSaveClicked)
										.HAlign(HAlign_Center)
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
												.Text(FText::FromString(TEXT("Save Event")))
												.Font(FStyleDefaults::GetFontInfo(20))
										]
								]
						]
				]
		];

	CreateMonthGrid();
	RefreshAllEventLists();

	for (int i = 0; i < 7; i++)
	{
		EDayEnum WeekDay = (EDayEnum)(i + 1);

		WeekDayPanel->AddSlot(i, 0)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FLinearColor(0.03f, 0.03f, 0.03f))
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
							.Text(FText::FromString(WeekDayEnum->GetNameStringByIndex(i + 1)))
							.Font(FStyleDefaults::GetFontInfo(20))
					]
			];
	}
}

TSharedRef<SWidget> SEventCollectionEditor::BuildDetailPanel()
{
	DetailViewWidget.Reset();

	bool bCreateWidget = true;
	FText MissingWidgetText = LOCTEXT("EditorWidget", "Editor Only");

	if (GIsEditor)
	{
		if (ViewedObject == nullptr)
		{
			MissingWidgetText = LOCTEXT("UndefinedObject", "Undefined Object");
		}
		else
		{
			FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

			FDetailsViewArgs DetailsViewArgs;
			DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
			DetailsViewArgs.bUpdatesFromSelection = false;
			DetailsViewArgs.bLockable = false;
			DetailsViewArgs.bShowPropertyMatrixButton = false;

			DetailViewWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

			DetailViewWidget->SetCustomFilterLabel(LOCTEXT("ShowAllParameters", "Show All Parameters"));
			DetailViewWidget->SetObject(ViewedObject);
			if (!DetailViewWidget.IsValid())
			{
				MissingWidgetText = LOCTEXT("UnknownError", "Unknown Error");
				bCreateWidget = false;
			}
		}
	}

	if (!bCreateWidget)
	{
		return SNew(STextBlock)
			.Text(MissingWidgetText);
	}
	else
		return DetailViewWidget.ToSharedRef();
}

FReply SEventCollectionEditor::IncreaseMonth()
{
	int NewMonth = CurrentDate.Month + 1;

	if (NewMonth > 12)
	{
		CurrentDate.Month = 1;
		CurrentDate.Year++;
	}
	else
	{
		CurrentDate.Month = NewMonth;
	}

	CreateMonthGrid();

	return FReply::Handled();
}

FReply SEventCollectionEditor::DecreaseMonth()
{
	int NewMonth = CurrentDate.Month - 1;

	if (NewMonth < 1)
	{
		CurrentDate.Month = 12;
		CurrentDate.Year--;
	}
	else
	{
		CurrentDate.Month = NewMonth;
	}

	CreateMonthGrid();

	return FReply::Handled();
}

void SEventCollectionEditor::CreateMonthGrid()
{
	DateText->SetText(FText::FromString(MonthStrings[CurrentDate.Month - 1] + " " + FString::FromInt(CurrentDate.Year)));

	// Reset everything
	DayEventLists.Empty();
	DaysWithEvents.Empty();
	WrapPanel->ClearChildren();
	ClearDetailPanel();
	EditingEvent = nullptr;
	ListWithSelection = nullptr;

	// The week day of the first day of the month, value is between 1-7
	int WeekDayOfFirst = UTimeDateUtility::EvaluateWeekDay(CurrentDate.Year, CurrentDate.Month, 1);

	FDate CalendarDate;

	// Determine if days from previous month need to be shown, 1 is Monday, if the first day of the month is not Monday, we will show some days from previous month to fill the first week row.
	if (WeekDayOfFirst > 1)
	{
		if (CurrentDate.Month - 1 == 0)
		{
			CalendarDate.Month = 12;
			CalendarDate.Year = CurrentDate.Year - 1;
		}
		else
		{
			CalendarDate.Month = CurrentDate.Month - 1;
			CalendarDate.Year = CurrentDate.Year;
		}

		CalendarDate.Day = UTimeDateUtility::EvaluateDaysInMonth(CalendarDate.Year, CalendarDate.Month) - WeekDayOfFirst + 2;
	}
	else
	{
		CalendarDate = CurrentDate;
		CalendarDate.Day = 1;
	}

	TimeFrame.Start.Date = CalendarDate;

	int TotalDaysWithPrevMonthSurplus = UTimeDateUtility::EvaluateDaysInMonth(CurrentDate.Year, CurrentDate.Month) + WeekDayOfFirst - 2;

	int Col = 7;
	// Add another row if total days of current month plus surplus from previous month is more than 35 (5 rows);
	int Row = TotalDaysWithPrevMonthSurplus >= 35 ? 6 : 5;

	TArray<FEventData>* AllEvents = nullptr;

	if (SelectedCollection != nullptr)
	{
		AllEvents = &SelectedCollection->Events;
	}
	
	for (int i = 0; i < Row * Col; i++)
	{
		TArray<TSharedPtr<FEventEntry>> EventInDay;
		if (AllEvents)
		{
			for (int Index = 0; Index < AllEvents->Num(); Index++)
			{
				FEventData& Data = (*AllEvents)[Index];
				if (Data.TriggerTime.Date == CalendarDate)
				{
					EventInDay.Add(MakeShareable(new FEventEntry(&Data)));
				}
			}
		}

		TSharedPtr<SDayEventList> DayEventList;

		int CurrentRow = i / Col;
		int CurrentCol = i % Col;
		WrapPanel->AddSlot(CurrentCol, CurrentRow)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(DayEventList, SDayEventList)
					.Date(CalendarDate)
					.EventList(EventInDay)
					.EventCollection(SelectedCollection)
					.OnSelectionChangedDelegate(this, &SEventCollectionEditor::OnSelectionChanged)
					.OnCollectionSizeChangedDelegate(this, &SEventCollectionEditor::OnCollectionSizeChanged)
			];

		DayEventLists.Add(DayEventList);
		if(!EventInDay.IsEmpty())
			DaysWithEvents.Add(DayEventList);

		CalendarDate++;
	}

	TimeFrame.End.Date = CalendarDate;
}

void SEventCollectionEditor::RefreshDetailPanel()
{
	ViewedObject->TriggerTime = EditingEvent->TriggerTime;
	ViewedObject->RepeatList.RepeatingDays = EditingEvent->RepeatingDays;
	ViewedObject->Event = EditingEvent->Event;

	UpdateDataList();
}

void SEventCollectionEditor::RefreshAllEventLists()
{
	ListItems.Empty();
	for (auto& Event : SelectedCollection->Events)
	{
		bool AddBetween = false;
		for(int i = 0; i < ListItems.Num(); i++)
			if (Event.TriggerTime < ListItems[i]->EventData->TriggerTime)
			{
				ListItems.Insert(MakeShareable(new FEventEntry(&Event)), i);
				AddBetween = true;
				break;
			}

		if(!AddBetween)
			ListItems.Add(MakeShareable(new FEventEntry(&Event)));
	}

	AllEventListWidget->RequestListRefresh();
}

void SEventCollectionEditor::ClearDetailPanel()
{
	ViewedObject->TriggerTime = FTimeDate::ZeroTimeDate;
	ViewedObject->RepeatList.RepeatingDays = { false, false, false, false, false, false, false };
	ViewedObject->Event.SetTagName(FName());
	ViewedObject->DataContainer.Data.Empty();

	if (ListWithSelection)
		ListWithSelection->ClearSelection();
	ListWithSelection = nullptr;

	EditingEvent = nullptr;

	DetailViewWidget->ForceRefresh();
}

void SEventCollectionEditor::OnCollectionSorted()
{
	// If the sorted event is triggered from a editor save, we don't want to do a clean refresh on the calendar grid
	if (SavedFromEditor)
	{
		SavedFromEditor = false;
		return;
	}

	EditingEvent = nullptr;
	ListWithSelection = nullptr;

	CreateMonthGrid();
}

void SEventCollectionEditor::OnSelectionChanged(TSharedPtr<FEventEntry> NewSelection, SDayEventList* ListView)
{
	if (!NewSelection)
		return;

	if (!NewSelection->EventData)
		return;

	EditingEvent = NewSelection->EventData;

	if (ListWithSelection)
		ListWithSelection->ClearSelection();

	AllEventListWidget->ClearSelection();

	ListWithSelection = ListView;

	RefreshDetailPanel();
}

void SEventCollectionEditor::OnSelectionChanged(TSharedPtr<FEventEntry> NewSelection, ESelectInfo::Type SelectionType)
{
	if (!NewSelection)
		return;

	if (!NewSelection->EventData)
		return;

	EditingEvent = NewSelection->EventData;

	if (ListWithSelection)
		ListWithSelection->ClearSelection();

	ListWithSelection = nullptr;

	RefreshDetailPanel();
}

FReply SEventCollectionEditor::OnSaveClicked()
{
	if (!EditingEvent)
		return FReply::Handled();

	if (EditingEvent->TriggerTime != ViewedObject->TriggerTime)
	{
		if(ListWithSelection)
			ListWithSelection->RemoveItem(EditingEvent);

		if (TimeFrame.IsInTimeFrame(ViewedObject->TriggerTime))
		{
			for (auto& DayEventList : DayEventLists)
			{
				if (TSharedPtr<SDayEventList> CycledList = DayEventList.Pin())
				{
					if (CycledList->GetDate() == ViewedObject->TriggerTime.Date)
					{
						CycledList->AddItem(EditingEvent);
						ListWithSelection = CycledList.Get();
						break;
					}
				}
			}
		}
	}

	FScopedTransaction Transaction(LOCTEXT("EventCollectionEditor_SaveEventData", "Save Current Event Edit"));

	if (SelectedCollection)
	{
		SelectedCollection->Modify();

		FName OldEventName = EditingEvent->Event.GetTagName();
		FName NewEventName = ViewedObject->Event.GetTagName();

		EditingEvent->TriggerTime = ViewedObject->TriggerTime;
		EditingEvent->RepeatingDays = ViewedObject->RepeatList.RepeatingDays;
		EditingEvent->Event = ViewedObject->Event;
		EditingEvent->DataContainer = ViewedObject->DataContainer;

		// Create the annoucement delegate for new events
		if (FindFProperty<FMulticastInlineDelegateProperty>(AnnouncerBlueprint->GeneratedClass, NewEventName))
		{
			TArray<FDataTag> NewParams;
			ViewedObject->DataContainer.Data.GetKeys(NewParams);

			UEventDelegateUtility::UpdateDispatcherParameters(AnnouncerBlueprint, NewEventName.ToString(), NewParams, ViewedObject);
		}
		else if (ViewedObject->Event.GetTagName() != NAME_None)
		{
			TArray<FDataTag> Params;
			ViewedObject->DataContainer.Data.GetKeys(Params);

			UEventDelegateUtility::CreateEventDispatcher(AnnouncerBlueprint, NewEventName.ToString(), Params);
		}

		if (OldEventName != NewEventName)
		{
			if (SelectedCollection->EventDelegateReferenceCounts.Contains(OldEventName))
			{
				SelectedCollection->EventDelegateReferenceCounts[OldEventName]--;

				if (SelectedCollection->EventDelegateReferenceCounts[OldEventName] <= 0)
				{
					SelectedCollection->EventDelegateReferenceCounts.Remove(OldEventName);
					UEventDelegateUtility::DeleteEventDispatcher(AnnouncerBlueprint, OldEventName);
				}
			}
			if (!SelectedCollection->EventDelegateReferenceCounts.Contains(NewEventName))
				SelectedCollection->EventDelegateReferenceCounts.Add(NewEventName, 0);
			SelectedCollection->EventDelegateReferenceCounts[NewEventName]++;
		}
		
		UPackage* Package = SelectedCollection->GetPackage();
		UPackage* AnnouncerPackage = AnnouncerBlueprint->GetPackage();
		if (Package)
		{
			UEditorLoadingAndSavingUtils::SavePackages({ Package, AnnouncerPackage }, true);
			SavedFromEditor = true;
		}
	}
	else
		Transaction.Cancel();

	if(ListWithSelection)
		ListWithSelection->RefreshItemLabels();

	RefreshAllEventLists();

	return FReply::Handled();
}

void SEventCollectionEditor::OnTextCommited(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction != ETextCommit::Type::OnEnter)
		return;

	TArray<FString> Parsed;
	InText.ToString().ParseIntoArray(Parsed, TEXT("/"));

	if(Parsed.Num() == 2)
	{
		int ParsedYear = FCString::Atoi(*Parsed[0]);
		int ParsedMonth = FCString::Atoi(*Parsed[1]);
		if (ParsedYear >= 0 && ParsedMonth > 0 && ParsedMonth <= 12)
		{
			CurrentDate.Year = ParsedYear;
			CurrentDate.Month = ParsedMonth;
			CreateMonthGrid();
		}
	}	
}

void SEventCollectionEditor::OnDateFieldFocused(
	const FFocusEvent& InFocusEvent, 
	const FWeakWidgetPath& InOldWidgetPath, 
	const TSharedPtr<SWidget>& InOldWidget, 
	const FWidgetPath& InNewWidgetPath, 
	const TSharedPtr<SWidget>& InNewWidget)
{
	// Check if the newly focused widget is our SEditableText
	if (InNewWidget.Get() == DateText.Get())
	{
		DateText->SetText(FText::FromString(FString::FromInt(CurrentDate.Year) + "/" + FString::FromInt(CurrentDate.Month)));
	}
}

TSharedRef<ITableRow> SEventCollectionEditor::OnGenerateRowForList(TSharedPtr<FEventEntry> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	FNumberFormattingOptions NoGrouping;
	NoGrouping.UseGrouping = false;

	FString RepeatString;

	for (int i = 0; i < Item->EventData->RepeatingDays.Num(); i++)
		if (Item->EventData->RepeatingDays[i])
			RepeatString += WeekDayStrings[i] + " ";

	FDate Date = Item->EventData->TriggerTime.Date;
	FString Label = FString::Printf(TEXT("%02i %-9s %04i | %-20s | %s"), Date.Day, *MonthStrings[Date.Month - 1], Date.Year, *Item->EventData->Event.GetTagName().ToString(), *RepeatString);

	TSharedPtr<STextBlock> TextBlock = SNew(STextBlock)
		.Text(FText::FromString(Label))
		.Font(FCoreStyle::GetDefaultFontStyle("Mono", 10));

	return SNew(STableRow< TSharedPtr<FEventEntry>>, OwnerTable)
		.Padding(2.0f)
		[
			TextBlock.ToSharedRef()
		];
}

void SEventCollectionEditor::OnCollectionSizeChanged(FDate ModifiedDate, FName Event)
{
	ClearDetailPanel();

	if (Event != NAME_None)
	{
		if (SelectedCollection->EventDelegateReferenceCounts.Contains(Event))
		{
			SelectedCollection->EventDelegateReferenceCounts[Event]--;

			if (SelectedCollection->EventDelegateReferenceCounts[Event] <= 0)
			{
				SelectedCollection->EventDelegateReferenceCounts.Remove(Event);
				UEventDelegateUtility::DeleteEventDispatcher(AnnouncerBlueprint, Event);
			}
		}
	}

	// Add the list with event being added/removed into the list.
	// Since I have a unsorted event collection, filtering out which days had or will have event before hand will reduce
	// The amount of times we have to cycle through the events.
	TSharedPtr<SDayEventList> ModifiedDay;
	for (auto& List : DayEventLists)
		if (ModifiedDay = List.Pin())
			if (ModifiedDay->GetDate() == ModifiedDate)
				DaysWithEvents.AddUnique(ModifiedDay); // Make sure same list doesn't get added again

	for(auto& ListWithEvent : DaysWithEvents)
	{
		TSharedPtr<SDayEventList> CycledList = ListWithEvent.Pin();
		if(!CycledList.IsValid())
			continue;

		// Only repopulate a list if the date is before the date of the event being removed because address of event after the one removed is shifted forward
		CycledList->ClearItems();

		TArray<TSharedPtr<FEventEntry>> EventsInDay;
		for(auto& Event : SelectedCollection->Events)
		{
			if (Event.TriggerTime.Date == CycledList->GetDate())
			{
				EventsInDay.Add(MakeShareable(new FEventEntry(&Event)));
			}
		}

		CycledList->SetItems(EventsInDay);
	}

	DaysWithEvents.Empty();
	for (auto& List : DayEventLists)
		if (TSharedPtr<SDayEventList> ListPtr = List.Pin())
			if (ListPtr->GetNumItems() > 0)
				DaysWithEvents.Add(ListPtr);

	RefreshAllEventLists();
}

TSharedPtr<SWidget> SEventCollectionEditor::OnContextMenuOpening()
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

void SEventCollectionEditor::AddNewEvent()
{
	if (!SelectedCollection)
		return;

	FEventData NewEventData;

	SelectedCollection->Events.Add(NewEventData);

	if (TimeFrame.IsInTimeFrame(FTimeDate()))
		OnCollectionSizeChanged(FDate(), NAME_None);
	else
		RefreshAllEventLists();
}

void SEventCollectionEditor::RemoveEvent()
{
	if (!SelectedCollection)
		return;

	TSharedPtr<FEventEntry> SelectedItem = AllEventListWidget->GetSelectedItems()[0];
	FTimeDate RemoveDate = SelectedItem->EventData->TriggerTime;
	FName EventName = SelectedItem->EventData->Event.GetTagName();

	if (SelectedCollection->EventDelegateReferenceCounts.Contains(EventName))
	{
		SelectedCollection->EventDelegateReferenceCounts[EventName]--;

		if (SelectedCollection->EventDelegateReferenceCounts[EventName] <= 0)
		{
			SelectedCollection->EventDelegateReferenceCounts.Remove(EventName);
			UEventDelegateUtility::DeleteEventDispatcher(AnnouncerBlueprint, EventName);
		}
	}

	for (int i = 0; i < SelectedCollection->Events.Num(); i++)
	{
		EventName = SelectedCollection->Events[i].Event.GetTagName();

		if (&SelectedCollection->Events[i] == SelectedItem->EventData)
		{
			SelectedCollection->Events.RemoveAt(i);
			break;
		}
	}

	if (TimeFrame.IsInTimeFrame(RemoveDate))
		OnCollectionSizeChanged(RemoveDate.Date, EventName);
	else
		RefreshAllEventLists();
}

bool SEventCollectionEditor::CanRemoveEvent()
{
	return AllEventListWidget->GetNumItemsSelected() > 0;
}

void SEventCollectionEditor::UpdateDataList()
{
	const UEventTagSettings* Settings = GetMutableDefault<UEventTagSettings>();
	if(!Settings)
		return;

	ViewedObject->DataContainer.Data.Empty();

	for(auto& Event : Settings->EventList)
		if (Event.Event == ViewedObject->Event.GetTagName().ToString())
		{
			for (auto& Data : Event.Data)
			{
				if (Data.IsEmpty())
					continue;

				FDataTag DataTag(*Data);
				double value = 0;
				if(EditingEvent)
					value = EditingEvent->DataContainer.Data.Contains(DataTag) ? EditingEvent->DataContainer.Data[DataTag] : 0;

				ViewedObject->DataContainer.Data.Add(DataTag, value);
			}
			break;
		}

	DetailViewWidget->ForceRefresh();
}

#undef LOCTEXT_NAMESPACE
