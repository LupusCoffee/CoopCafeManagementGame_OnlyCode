// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GOAPSystem : ModuleRules
{
	public GOAPSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
                                                                "Core",
                                                                "CoreUObject",
                                                                "Engine",
                                                                "InputCore",
                                                                "EnhancedInput",
                                                                "AIModule",
                                                                "DeveloperSettings",
                                                                "OnlineSubsystemSteam",
                                                                "OnlineSubsystem",
                                                                "Niagara",
                                                                "NavigationSystem",
                                                                "ProceduralMeshComponent",
                                                                "GameplayTags",
                                                                "TrackSystem"
        });
    }
}
