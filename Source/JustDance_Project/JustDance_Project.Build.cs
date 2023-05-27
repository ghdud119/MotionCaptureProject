// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JustDance_Project : ModuleRules
{
	public JustDance_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "Sockets", "Networking", "Json", "JsonUtilities" });
	}
}
