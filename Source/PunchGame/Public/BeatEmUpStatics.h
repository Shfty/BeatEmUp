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

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FName> GetInputAxisNames();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FName> GetInputActionNames();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FInputAxisKeyMapping> GetInputAxisMappings();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FInputActionKeyMapping> GetInputActionMappings();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FKey> GetInputKeysForAxis(FName Axis);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TArray<FKey> GetInputKeysForAction(FName Action);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void SetInputAxisMappings(TArray<FInputAxisKeyMapping> NewAxisMappings);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void SetInputActionMappings(TArray<FInputActionKeyMapping> NewActionMappings);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void RemapInputAxis(FName Axis, FKey OldKey, FKey NewKey);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void RemapInputAction(FName Action, FKey OldKey, FKey NewKey);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void ClearInputAxisMappings();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void ClearInputActionMappings();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void ApplyInputMappings(APlayerController* PlayerController);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static bool ValidateIPAddress(FString IPAddress);
	
};
