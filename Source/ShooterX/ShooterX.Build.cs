// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShooterX : ModuleRules
{
	public ShooterX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			// Initial Dependencies
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",

			// Json
            "Json", "JsonUtilities",

			// AI
			"AIModule", "NavigationSystem", "GameplayTasks",

			// UI
			"UMG",

        });

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"ShooterXPlayGround",
		});

		PublicIncludePaths.AddRange(new string[] { "ShooterX" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
