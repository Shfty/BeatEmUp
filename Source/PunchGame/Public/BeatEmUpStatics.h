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
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FString> GetAllMapNamesInFolder(FString Folder, bool Recursive = false);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FString> GetAllMapNames();
	
};
