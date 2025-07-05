// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DevakiVillage : ModuleRules
{
	public DevakiVillage(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PublicIncludePaths.AddRange(new string[] {
			"DevakiVillage",
			"DevakiVillage/Interfaces"
		});

	}
}
