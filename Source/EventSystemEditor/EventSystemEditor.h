// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FEventSystemEditor : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedRef<class SDockTab> OnSpawnEventCollectionEditor(const class FSpawnTabArgs& SpawnTabArgs);
    void EventCollectionEditorButtonClicked();
    void RegisterMenus();
};