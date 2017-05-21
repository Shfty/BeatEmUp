// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomProjectionFunctionLibrary.generated.h"

UCLASS()
class CUSTOMPROJECTION_API UCustomProjectionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SetCustomProjectionMatrix(FMatrix NewProjectionMatrix);

	UFUNCTION(BlueprintPure)
	static FMatrix CombineMatrices(FMatrix A, FMatrix B);

	UFUNCTION(BlueprintPure)
	static FMatrix InterpolateMatrices(FMatrix A, FMatrix B, float Alpha);

	UFUNCTION(BlueprintPure)
	static FMatrix MakeTranslationMatrix(FVector Delta);

	UFUNCTION(BlueprintPure)
	static FMatrix MakeRotationMatrix(FRotator Delta);

	UFUNCTION(BlueprintPure)
	static FMatrix MakePerspectiveMatrix(const float HalfFOV = 45.0f, const float Width = 640.0f, const float Height = 480.0f, const float NearPlane = 0.01f, const bool ReverseZ = true);

	UFUNCTION(BlueprintPure)
	static FMatrix MakeOrthoMatrix(const float Width = 640.0f, const float Height = 480.0f, const float NearPlane = 0.0f, const float FarPlane = 10000.0f, const bool ReverseZ = true);

	UFUNCTION(BlueprintPure)
	static FMatrix MakeShearMatrix(const float ShearX = 0.0f, const float ShearY = 0.0f);

	UFUNCTION(BlueprintPure)
	static FMatrix MakeCavalierMatrix(const float Width = 640.0f, const float Height = 480.0f, const float NearPlane = 0.0f, const float FarPlane = 10000.0f, const float ShearX = 0.0f, const float ShearY = 0.0f);

	UFUNCTION(BlueprintPure)
	static bool CustomDeprojectScreenToWorld(APlayerController const* Player, const FVector2D& ScreenPosition, const FMatrix& ProjectionMatrix, FVector& WorldPosition, FVector& WorldDirection);

};
