// ShooterX.Build.cs

using UnrealBuildTool;

public class ShooterX : ModuleRules
{
	public ShooterX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ 
				// Initial Dependencies.
				"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",

				// Json Modules
				"Json", "JsonUtilities",

				// Input
				"EnhancedInput",

				// Niagara
				"Niagara",

				// AI
				"AIModule", "NavigationSystem",
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] { "ShooterX" });
	}
}
