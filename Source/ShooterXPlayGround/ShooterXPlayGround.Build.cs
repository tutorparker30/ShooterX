// ShooterXPlayGround.Build.cs

using UnrealBuildTool;

public class ShooterXPlayGround : ModuleRules
{
	public ShooterXPlayGround(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{
				// Initial Modules
				"Core", "CoreUObject", "Engine", "InputCore",
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[] { });

	}
}
