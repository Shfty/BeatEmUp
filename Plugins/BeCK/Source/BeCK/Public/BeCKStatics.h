// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BeCKStatics.generated.h"

/**
 * 
 */
UCLASS()
class BECK_API UBeCKStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Beam Em Up")
	static float GetPlatformTimeSeconds();
	
};
