// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorUtilityWidgetExtension.h"
#include "ToolMenus.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

IMPLEMENT_GAME_MODULE(FEditorUtilityWidgetExtension, EditorUtilityWidgetExtension);

void FEditorUtilityWidgetExtension::StartupModule()
{
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEditorUtilityWidgetExtension::RegisterMenuExtension));
}

void FEditorUtilityWidgetExtension::ShutdownModule()
{
    // Unregister the startup function
    UToolMenus::UnRegisterStartupCallback(this);

    // Unregister all our menu extensions
    UToolMenus::UnregisterOwner(this);
}

void FEditorUtilityWidgetExtension::RegisterMenuExtension()
{
    // Use the current object as the owner of the menus
    // This allows us to remove all our custom menus when the
    // module is unloaded (see ShutdownModule below)
    FToolMenuOwnerScoped OwnerScoped(this);

    // Extend the "File" section of the main toolbar
    UToolMenu* AssetsToolBar = UToolMenus::Get()->ExtendMenu(
        "LevelEditor.LevelEditorToolBar.AssetsToolBar");
    FToolMenuSection& ToolbarSection = AssetsToolBar->FindOrAddSection("Content");

    ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(
        TEXT("ItemEditor"),
        FExecuteAction::CreateLambda([]()
            {
                UObject* WidgetObj = LoadObject<UObject>(nullptr, TEXT("/Game/Tools/ItemEditor"));
                UEditorUtilityWidgetBlueprint* WidgetBP = Cast<UEditorUtilityWidgetBlueprint>(WidgetObj);

                if (WidgetBP)
                {
                    UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
                    EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBP);
                }
            }),
        INVTEXT("Item Editor"),
        INVTEXT("Open the item editor"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Sequencer.AllowSequencerEditsOnly")
    ));
}
