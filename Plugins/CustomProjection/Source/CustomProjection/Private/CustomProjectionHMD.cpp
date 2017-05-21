// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CustomProjectionHMD.h"
#include "Misc/App.h"
#include "Modules/ModuleManager.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "ICustomProjectionPlugin.h"
#include "SceneRendering.h"
#include "PostProcess/PostProcessHMD.h"

#include "CustomProjectionFunctionLibrary.h"

FMatrix FCustomProjectionHMD::CustomMatrix = UCustomProjectionFunctionLibrary::MakePerspectiveMatrix();

//---------------------------------------------------
// CustomProjectionHMD Plugin Implementation
//---------------------------------------------------

class FCustomProjectionPlugin : public ICustomProjectionPlugin
{
	/** IHeadMountedDisplayModule implementation */
	virtual TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > CreateHeadMountedDisplay() override;

	FString GetModuleKeyName() const override
	{
		return FString(TEXT("Custom Projection HMD"));
	}
};

IMPLEMENT_MODULE( FCustomProjectionPlugin, CustomProjection )

TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > FCustomProjectionPlugin::CreateHeadMountedDisplay()
{
	TSharedPtr< FCustomProjectionHMD, ESPMode::ThreadSafe > CustomProjectionHMD( new FCustomProjectionHMD() );
	if(CustomProjectionHMD->IsInitialized() )
	{
		return CustomProjectionHMD;
	}
	return nullptr;
}


//---------------------------------------------------
// CustomProjectionHMD IHeadMountedDisplay Implementation
//---------------------------------------------------

bool FCustomProjectionHMD::IsHMDEnabled() const
{
	return true;
}

void FCustomProjectionHMD::EnableHMD(bool enable)
{
}

EHMDDeviceType::Type FCustomProjectionHMD::GetHMDDeviceType() const
{
	return EHMDDeviceType::DT_ES2GenericStereoMesh;
}

bool FCustomProjectionHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	MonitorDesc.MonitorName = "";
	MonitorDesc.MonitorId = 0;
	MonitorDesc.DesktopX = MonitorDesc.DesktopY = MonitorDesc.ResolutionX = MonitorDesc.ResolutionY = 0;
	return false;
}

void FCustomProjectionHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	OutHFOVInDegrees = 0.0f;
	OutVFOVInDegrees = 0.0f;
}

bool FCustomProjectionHMD::DoesSupportPositionalTracking() const
{
	return false;
}

bool FCustomProjectionHMD::HasValidTrackingPosition()
{
	return false;
}

void FCustomProjectionHMD::GetPositionalTrackingCameraProperties(FVector& OutOrigin, FQuat& OutOrientation, float& OutHFOV, float& OutVFOV, float& OutCameraDistance, float& OutNearPlane, float& OutFarPlane) const
{
}

void FCustomProjectionHMD::RebaseObjectOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation) const
{
}

void FCustomProjectionHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
}

float FCustomProjectionHMD::GetInterpupillaryDistance() const
{
	return 0.0f;
}

void FCustomProjectionHMD::GetCurrentOrientationAndPosition(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	CurrentOrientation = FQuat::Identity;
	CurrentPosition = FVector::ZeroVector;
}

TSharedPtr<ISceneViewExtension, ESPMode::ThreadSafe> FCustomProjectionHMD::GetViewExtension()
{
	TSharedPtr<FCustomProjectionHMD, ESPMode::ThreadSafe> ptr(AsShared());
	return StaticCastSharedPtr<ISceneViewExtension>(ptr);
}

void FCustomProjectionHMD::ApplyHmdRotation(APlayerController* PC, FRotator& ViewRotation)
{

}

bool FCustomProjectionHMD::UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	return false;
}

bool FCustomProjectionHMD::IsChromaAbCorrectionEnabled() const
{
	return false;
}

bool FCustomProjectionHMD::Exec( UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar )
{
	return false;
}

bool FCustomProjectionHMD::IsPositionalTrackingEnabled() const
{
	return false;
}

bool FCustomProjectionHMD::EnablePositionalTracking(bool enable)
{
	return false;
}

bool FCustomProjectionHMD::IsHeadTrackingAllowed() const
{
	return true;
}

bool FCustomProjectionHMD::IsInLowPersistenceMode() const
{
	return false;
}

void FCustomProjectionHMD::EnableLowPersistenceMode(bool Enable)
{
}

void FCustomProjectionHMD::ResetOrientationAndPosition(float yaw)
{
}

void FCustomProjectionHMD::ResetOrientation(float Yaw)
{
}
void FCustomProjectionHMD::ResetPosition()
{
}

void FCustomProjectionHMD::SetClippingPlanes(float NCP, float FCP)
{
}

void FCustomProjectionHMD::SetBaseRotation(const FRotator& BaseRot)
{
}

FRotator FCustomProjectionHMD::GetBaseRotation() const
{
	return FRotator::ZeroRotator;
}

void FCustomProjectionHMD::SetBaseOrientation(const FQuat& BaseOrient)
{
}

FQuat FCustomProjectionHMD::GetBaseOrientation() const
{
	return FQuat::Identity;
}

bool FCustomProjectionHMD::IsStereoEnabled() const
{
	return true;
}

bool FCustomProjectionHMD::EnableStereo(bool stereo)
{
	return true;
}

void FCustomProjectionHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
	// Zero out the right eye view rect as we only need one viewport
	if( StereoPass == eSSP_RIGHT_EYE )
	{
		X = 0;
		Y = 0;
		SizeX = 0;
		SizeY = 0;
	}
}

void FCustomProjectionHMD::CalculateStereoViewOffset(const enum EStereoscopicPass StereoPassType, const FRotator& ViewRotation, const float WorldToMeters, FVector& ViewLocation)
{

}

FMatrix FCustomProjectionHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType, const float FOV) const
{
	return CustomMatrix;
}

void FCustomProjectionHMD::InitCanvasFromView(FSceneView* InView, UCanvas* Canvas)
{
}

void FCustomProjectionHMD::GetEyeRenderParams_RenderThread(const FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
{
	EyeToSrcUVOffsetValue = FVector2D::ZeroVector;
	EyeToSrcUVScaleValue = FVector2D(1.0f, 1.0f);
}


void FCustomProjectionHMD::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	InViewFamily.EngineShowFlags.HMDDistortion = true;
	InViewFamily.EngineShowFlags.SetScreenPercentage(true);
	InViewFamily.EngineShowFlags.StereoRendering = IsStereoEnabled();
}

void FCustomProjectionHMD::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	InView.BaseHmdOrientation = FQuat(FRotator(0.0f,0.0f,0.0f));
	InView.BaseHmdLocation = FVector(0.f);
	InViewFamily.bUseSeparateRenderTarget = false;
}

void FCustomProjectionHMD::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
	check(IsInRenderingThread());
}

void FCustomProjectionHMD::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	check(IsInRenderingThread());
}

FCustomProjectionHMD::FCustomProjectionHMD()
{
}

FCustomProjectionHMD::~FCustomProjectionHMD()
{
}

bool FCustomProjectionHMD::IsInitialized() const
{
	return true;
}
