// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TrackSystem : ModuleRules
{
	public TrackSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		if(Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
        }
    }
}
