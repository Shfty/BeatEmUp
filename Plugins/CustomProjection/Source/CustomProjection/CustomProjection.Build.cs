// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class CustomProjection : ModuleRules
	{
		public CustomProjection(TargetInfo Target)
        {
            PrivateIncludePaths.AddRange(
				new string[] {
                    "D:/OtherPrograms/Epic Games/UE_4.15/Engine/Source/Runtime/Renderer/Private",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"RHI",
					"RenderCore",
					"Renderer",
					"ShaderCore",
					"HeadMountedDisplay"
				}
			);
		}
	}
}
