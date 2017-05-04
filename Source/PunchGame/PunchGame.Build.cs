// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class PunchGame : ModuleRules
{
	public PunchGame(TargetInfo Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
                "PunchGame/Public",
                "PunchGame/Public/AssetTypeActions"
            }
		);
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "PunchGame/Private",
                "PunchGame/Private/AssetTypeActions"
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
