// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomProjectionHMD.h"
#include "CustomProjectionFunctionLibrary.h"

#include "SceneView.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"


void UCustomProjectionFunctionLibrary::SetCustomProjectionMatrix(FMatrix NewProjectionMatrix)
{
	FCustomProjectionHMD::CustomMatrix = NewProjectionMatrix;
}

FMatrix UCustomProjectionFunctionLibrary::CombineMatrices(FMatrix A, FMatrix B)
{
	return A * B;
}

FMatrix UCustomProjectionFunctionLibrary::InterpolateMatrices(FMatrix A, FMatrix B, float Alpha)
{
	FMatrix Out;

	for(int32 x = 0; x < 4; ++x)
	{
		for(int32 y = 0; y < 4; ++y)
		{
			Out.M[x][y] = A.M[x][y] + Alpha * (B.M[x][y] - A.M[x][y]);
		}
	}

	return Out;
}

FMatrix UCustomProjectionFunctionLibrary::MakeTranslationMatrix(FVector Delta)
{
	return FTranslationMatrix(Delta);
}

FMatrix UCustomProjectionFunctionLibrary::MakeRotationMatrix(FRotator Delta)
{
	return FRotationMatrix(Delta);
}

FMatrix UCustomProjectionFunctionLibrary::MakePerspectiveMatrix(float HalfFOV, float Width, float Height, float NearPlane, bool ReverseZ)
{
	if(ReverseZ)
	{
		return FReversedZPerspectiveMatrix(HalfFOV, Width, Height, NearPlane);
	}

	return FPerspectiveMatrix(HalfFOV, Width, Height, NearPlane);
}

FMatrix UCustomProjectionFunctionLibrary::MakeOrthoMatrix(float Width, float Height, float NearPlane, float FarPlane, bool ReverseZ)
{
	if(ReverseZ)
	{
		return FReversedZOrthoMatrix(Width, Height, 1.0f / FarPlane, NearPlane);
	}

	return FOrthoMatrix(Width, Height, 1.0f / FarPlane, NearPlane);
}

FMatrix UCustomProjectionFunctionLibrary::MakeShearMatrix(float ShearX, float ShearY)
{
	return FMatrix(
		FPlane(1.0f, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, 1.0f, 0.0f, 0.0f),
		FPlane(ShearX, ShearY, 1.0f, 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

FMatrix UCustomProjectionFunctionLibrary::MakeCavalierMatrix(float Width, float Height, float NearPlane, float FarPlane, float ShearX, float ShearY)
{
	FMatrix ShearMatrix = MakeShearMatrix(ShearX, ShearY);
	FMatrix OrthoMatrix = MakeOrthoMatrix(Width, Height, NearPlane, FarPlane);

	return ShearMatrix * OrthoMatrix;
}

bool UCustomProjectionFunctionLibrary::CustomDeprojectScreenToWorld(APlayerController const* Player, const FVector2D& ScreenPosition, const FMatrix& ProjectionMatrix, FVector& WorldPosition, FVector& WorldDirection)
{
	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if(LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if(LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, ProjectionData))
		{
			FMatrix ViewMatrix = FTranslationMatrix(-ProjectionData.ViewOrigin) * ProjectionData.ViewRotationMatrix;
			FSceneView::DeprojectScreenToWorld(ScreenPosition, ProjectionData.GetConstrainedViewRect(), ViewMatrix.InverseFast(), ProjectionMatrix.InverseFast(), WorldPosition, WorldDirection);
			return true;
		}
	}

	// something went wrong, zero things and return false
	WorldPosition = FVector::ZeroVector;
	WorldDirection = FVector::ZeroVector;
	return false;
}
