// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "OrthoHMDFunctionLibrary.generated.h"

UCLASS()
class ORTHOHMD_API UOrthoHMDFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SetShearFactor(const FVector2D& InShearFactor);

	UFUNCTION(BlueprintCallable)
	static void SetShearOffset(const FVector2D& InShearOffset);

	UFUNCTION(BlueprintPure)
	static FVector2D& GetShearFactor();

	UFUNCTION(BlueprintPure)
	static FVector2D& GetShearOffset();

};
