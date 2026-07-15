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
				"AIModule", "NavigationSystem", "GameplayTasks",

				// UI
				"UMG", "Slate", "SlateCore",

				// Online Subsystem
				"OnlineSubsystem", "OnlineSubsystemUtils",

				// GameplayAbilitySystem
				"GameplayAbilities", "GameplayTags",

				// Networking
				"NetCore",

			}
		);

		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"ShooterXPlayGround",
			}
		);

		PublicIncludePaths.AddRange(new string[] { "ShooterX" });

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
			// OnlineSubsystem 모듈이나 OnlineSubsystemUtils 모듈은 실제로 코드에서
			// #include "OnlineSubsystem.h" 또는 #include "OnlineSubsystemUtils.h"를 통해 직접 참조되고 있음.
			// 즉, PublicDependencyModuleNames에 포함되어야 함. 컴파일 타임에 링킹되게끔 하기 위함.
			// 그러나 OnlineSubsystemNull 모듈은 실제로 코드에서 직접 참조되지 않음.
			// OnlineSubsystemNull 관련 클래스들을 직접 include해서 쓰고 있지 않음. 
			// ini 파일 설정을 보고, 런타임에 자동으로 어떤 구현체를 로드할지 결정됨. OnlineSubsystemSteam도 동일함.
	}
}
