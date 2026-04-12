// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TrackSystemVisualizer : ModuleRules
{
	public TrackSystemVisualizer(ReadOnlyTargetRules Target) : base(Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "TrackSystem" });

		PrivateDependencyModuleNames.AddRange(new string[] { "ComponentVisualizers", "UnrealEd", "TypedElementRuntime", "TypedElementFramework" });
    }
}
