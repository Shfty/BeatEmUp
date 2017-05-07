// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BeatEmUpStatics.generated.h"

/**
 * 
 */
UCLASS()
class PUNCHGAME_API UBeatEmUpStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Utility")
	static TArray<FString> GetAllMapNamesInFolder(FString Folder, bool Recursive = false);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static TArray<FString> GetAllMapNames();

	UFUNCTION(BlueprintPure, Category = "Utility")
	static bool ValidateIPAddress(FString IPAddress);
	
};
