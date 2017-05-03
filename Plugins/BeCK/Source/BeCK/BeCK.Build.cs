// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class BeCK : ModuleRules
{
	public BeCK(TargetInfo Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
                "BeCK/Public",
                "BeCK/Public/AssetTypeActions"
            }
		);
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "BeCK/Private",
                "BeCK/Private/AssetTypeActions"
            }
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
                "Paper2D",
                "Slate",
                "SlateCore",
                "UMG"
            }
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{

			}
		);
	}
}
