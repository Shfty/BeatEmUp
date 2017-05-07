// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class PunchGame : ModuleRules
{
	public PunchGame(TargetInfo Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
                "PunchGame/Public",
            }
		);
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "PunchGame/Private",
            }
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
                "Sockets"
            }
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{

			}
		);
	}
}
