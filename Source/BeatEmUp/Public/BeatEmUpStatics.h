// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BeatEmUpStatics.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API UBeatEmUpStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Beam Em Up")
	static float GetPlatformTimeSeconds();
	
};
