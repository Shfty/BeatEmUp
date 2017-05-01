// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "OrthoHMDDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Ortho HMD"))
class ORTHOHMD_API UOrthoHMDDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, config, Category = "Ortho HMD")
	FVector2D InitialShearFactor;

	UPROPERTY(EditAnywhere, config, Category = "Ortho HMD")
	FVector2D InitialShearOffset;
	
};
