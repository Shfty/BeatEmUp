// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class BeatEmUp : ModuleRules
{
	public BeatEmUp(TargetInfo Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
                "BeatEmUp/Public",
                "BeatEmUp/Public/AssetTypeActions"
            }
		);
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "BeatEmUp/Private",
                "BeatEmUp/Private/AssetTypeActions"
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
