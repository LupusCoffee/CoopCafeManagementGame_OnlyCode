// Copyright Epic Games, Inc. All Rights Reserved.

#include "EventSystemEditor.h"
#include "EventSystemStructs.h"
#include "PropertyEditorModule.h"
#include "EventTagCustomization.h"
#include "EventTagGraphPanelPinFactory.h"
#include "ISettingsModule.h"
#include "EventTagSettings.h"
#include "EventSystemEditorPublic.h"
#include "EventSystemEditorStyle.h"

IMPLEMENT_GAME_MODULE(FEventSystemEditor, EventSystemEditor);

static const FName EventCollectionEditorTabName("EventCollectionEditor");

#define LOCTEXT_NAMESPACE "EventSystemEditor"

void FEventSystemEditor::StartupModule()
{
	FEventSystemEditorStyle::Initialize();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FName EventTagStructName = FEventTag::StaticStruct()->GetFName();
	PropertyModule.RegisterCustomPropertyTypeLayout(EventTagStructName, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEventSystemEditorPublic::MakeTagCustomizationWidget));

	FName DataTagStructName = FDataTag::StaticStruct()->GetFName();
	PropertyModule.RegisterCustomPropertyTypeLayout(DataTagStructName, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEventSystemEditorPublic::MakeTagCustomizationWidget));

	FName DateStructName = FDate::StaticStruct()->GetFName();
	PropertyModule.RegisterCustomPropertyTypeLayout(DateStructName, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEventSystemEditorPublic::MakeDateCustomizationWidget));

	FName TimeStructName = FTime::StaticStruct()->GetFName();
	PropertyModule.RegisterCustomPropertyTypeLayout(TimeStructName, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEventSystemEditorPublic::MakeTimeCustomizationWidget));

	FName RepeatListStructName = FRepeatList::StaticStruct()->GetFName();
	PropertyModule.RegisterCustomPropertyTypeLayout(RepeatListStructName, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEventSystemEditorPublic::MakeRepeatListCustomizationWidget));

	TSharedPtr<FEventTagsGraphPanelPinFactory> EventTagsGraphPanelPinFactory = MakeShareable(new FEventTagsGraphPanelPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(EventTagsGraphPanelPinFactory);

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Game", "EventTags",
			LOCTEXT("EventTagSettingsName", "EventTags"),
			LOCTEXT("EventTagSettingsNameDesc", "EventTag Settings"),
			GetMutableDefault<UEventTagSettings>()
		);
	}

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEventSystemEditor::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(EventCollectionEditorTabName, FOnSpawnTab::CreateRaw(this, &FEventSystemEditor::OnSpawnEventCollectionEditor))
		.SetDisplayName(LOCTEXT("EventCollectionEditorTabTitle", "Event Collection Editor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FEventSystemEditor::ShutdownModule()
{
	FEventSystemEditorStyle::Shutdown();

	UToolMenus::UnRegisterStartupCallback(this);

	// Unregister all our menu extensions
	UToolMenus::UnregisterOwner(this);
}

TSharedRef<class SDockTab> FEventSystemEditor::OnSpawnEventCollectionEditor(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			FEventSystemEditorPublic::MakeEventCollectionEditorWidget()
		];
}

void FEventSystemEditor::EventCollectionEditorButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(EventCollectionEditorTabName);
}

void FEventSystemEditor::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	// Extend the "File" section of the main toolbar
	UToolMenu* AssetsToolBar = UToolMenus::Get()->ExtendMenu(
		"LevelEditor.LevelEditorToolBar.AssetsToolBar");
	FToolMenuSection& ToolbarSection = AssetsToolBar->FindOrAddSection("Content");

	ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(
		TEXT("EventCollectionEditor"),
		FExecuteAction::CreateRaw(this, &FEventSystemEditor::EventCollectionEditorButtonClicked),
		INVTEXT("Event Collection Editor"),
		INVTEXT("Being up Event Collection Editor Window"),
		FSlateIcon(FEventSystemEditorStyle::GetStyleSetName(), "EventSystemEditor.CalendarIcon")
	));
}

#undef LOCTEXT_NAMESPACE