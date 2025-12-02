// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class EditorUtilityWidgetExtension : ModuleRules
{
	public EditorUtilityWidgetExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UMG", "UMGEditor", "Slate", "SlateCore", "ToolMenus", "Blutility" });

        if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
