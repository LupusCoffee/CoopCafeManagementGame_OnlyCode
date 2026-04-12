// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class EventSystemEditor : ModuleRules
{
	public EventSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "EventSystem", "GraphEditor", "BlueprintGraph", "EditorUtilityWidgetExtension", "ToolMenus" });

		PrivateDependencyModuleNames.AddRange(new string[] { "InputCore", "UnrealEd" });
    }
}
