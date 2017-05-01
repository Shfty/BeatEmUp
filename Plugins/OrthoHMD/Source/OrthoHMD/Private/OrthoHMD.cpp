// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OrthoHMD.h"
#include "Misc/App.h"
#include "Modules/ModuleManager.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "IOrthoHMDPlugin.h"
#include "SceneRendering.h"
#include "PostProcess/PostProcessHMD.h"

#include "OrthoHMDDeveloperSettings.h"

FVector2D FOrthoHMD::ShearFactor = FVector2D::ZeroVector;
FVector2D FOrthoHMD::ShearOffset = FVector2D::ZeroVector;

//---------------------------------------------------
// OrthoHMD Plugin Implementation
//---------------------------------------------------

class FOrthoHMDPlugin : public IOrthoHMDPlugin
{
	/** IHeadMountedDisplayModule implementation */
	virtual TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > CreateHeadMountedDisplay() override;

	FString GetModuleKeyName() const override
	{
		return FString(TEXT("OrthoHMD"));
	}
};

IMPLEMENT_MODULE( FOrthoHMDPlugin, OrthoHMD )

TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > FOrthoHMDPlugin::CreateHeadMountedDisplay()
{
	TSharedPtr< FOrthoHMD, ESPMode::ThreadSafe > OrthoHMD( new FOrthoHMD() );
	if( OrthoHMD->IsInitialized() )
	{
		return OrthoHMD;
	}
	return nullptr;
}


//---------------------------------------------------
// OrthoHMD IHeadMountedDisplay Implementation
//---------------------------------------------------

bool FOrthoHMD::IsHMDEnabled() const
{
	return true;
}

void FOrthoHMD::EnableHMD(bool enable)
{
}

EHMDDeviceType::Type FOrthoHMD::GetHMDDeviceType() const
{
	return EHMDDeviceType::DT_ES2GenericStereoMesh;
}

bool FOrthoHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	MonitorDesc.MonitorName = "";
	MonitorDesc.MonitorId = 0;
	MonitorDesc.DesktopX = MonitorDesc.DesktopY = MonitorDesc.ResolutionX = MonitorDesc.ResolutionY = 0;
	return false;
}

void FOrthoHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	OutHFOVInDegrees = 0.0f;
	OutVFOVInDegrees = 0.0f;
}

bool FOrthoHMD::DoesSupportPositionalTracking() const
{
	return false;
}

bool FOrthoHMD::HasValidTrackingPosition()
{
	return false;
}

void FOrthoHMD::GetPositionalTrackingCameraProperties(FVector& OutOrigin, FQuat& OutOrientation, float& OutHFOV, float& OutVFOV, float& OutCameraDistance, float& OutNearPlane, float& OutFarPlane) const
{
}

void FOrthoHMD::RebaseObjectOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation) const
{
}

void FOrthoHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
}

float FOrthoHMD::GetInterpupillaryDistance() const
{
	return 0.0f;
}

void FOrthoHMD::GetCurrentPose(FQuat& CurrentOrientation)
{
	CurrentOrientation = FQuat(FRotator(0.0f, 0.0f, 0.0f));
}

void FOrthoHMD::GetCurrentOrientationAndPosition(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	CurrentPosition = FVector(0.0f, 0.0f, 0.0f);

	GetCurrentPose(CurrentOrientation);
	CurHmdOrientation = LastHmdOrientation = CurrentOrientation;
}

TSharedPtr<ISceneViewExtension, ESPMode::ThreadSafe> FOrthoHMD::GetViewExtension()
{
	TSharedPtr<FOrthoHMD, ESPMode::ThreadSafe> ptr(AsShared());
	return StaticCastSharedPtr<ISceneViewExtension>(ptr);
}

void FOrthoHMD::ApplyHmdRotation(APlayerController* PC, FRotator& ViewRotation)
{
	ViewRotation.Normalize();

	GetCurrentPose(CurHmdOrientation);
	LastHmdOrientation = CurHmdOrientation;

	const FRotator DeltaRot = ViewRotation - PC->GetControlRotation();
	DeltaControlRotation = (DeltaControlRotation + DeltaRot).GetNormalized();

	// Pitch from other sources is never good, because there is an absolute up and down that must be respected to avoid motion sickness.
	// Same with roll.
	DeltaControlRotation.Pitch = 0;
	DeltaControlRotation.Roll = 0;
	DeltaControlOrientation = DeltaControlRotation.Quaternion();

	ViewRotation = FRotator(DeltaControlOrientation * CurHmdOrientation);
}

bool FOrthoHMD::UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	return false;
}

bool FOrthoHMD::IsChromaAbCorrectionEnabled() const
{
	return false;
}

bool FOrthoHMD::Exec( UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar )
{
	return false;
}

bool FOrthoHMD::IsPositionalTrackingEnabled() const
{
	return false;
}

bool FOrthoHMD::EnablePositionalTracking(bool enable)
{
	return false;
}

bool FOrthoHMD::IsHeadTrackingAllowed() const
{
	return true;
}

bool FOrthoHMD::IsInLowPersistenceMode() const
{
	return false;
}

void FOrthoHMD::EnableLowPersistenceMode(bool Enable)
{
}

void FOrthoHMD::ResetOrientationAndPosition(float yaw)
{
}

void FOrthoHMD::ResetOrientation(float Yaw)
{
}
void FOrthoHMD::ResetPosition()
{
}

void FOrthoHMD::SetClippingPlanes(float NCP, float FCP)
{
}

void FOrthoHMD::SetBaseRotation(const FRotator& BaseRot)
{
}

FRotator FOrthoHMD::GetBaseRotation() const
{
	return FRotator::ZeroRotator;
}

void FOrthoHMD::SetBaseOrientation(const FQuat& BaseOrient)
{
}

FQuat FOrthoHMD::GetBaseOrientation() const
{
	return FQuat::Identity;
}

bool FOrthoHMD::IsStereoEnabled() const
{
	return true;
}

bool FOrthoHMD::EnableStereo(bool stereo)
{
	return true;
}

void FOrthoHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
	if( StereoPass == eSSP_RIGHT_EYE )
	{
		X = 0;
		Y = 0;
		SizeX = 0;
		SizeY = 0;
	}
}

void FOrthoHMD::CalculateStereoViewOffset(const enum EStereoscopicPass StereoPassType, const FRotator& ViewRotation, const float WorldToMeters, FVector& ViewLocation)
{

}

FMatrix FOrthoHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType, const float FOV) const
{
	const float OrthoWidth = 320.0f;
	const float OrthoHeight = 180.0f;
	const float NearPlane = 0.0f;
	const float FarPlane = 2097152.0f;

	FMatrix ShearMatrix = FMatrix(
		FPlane(1.0f,			0.0f,			0.0f,			0.0f),
		FPlane(0.0f,			1.0f,			0.0f,			0.0f),
		FPlane(ShearFactor.X, ShearFactor.Y,	1.0f,			0.0f),
		FPlane(0.0f,			0.0f,			0.0f,			1.0f)
	);

	FMatrix OrthoMatrix = FReversedZOrthoMatrix(OrthoWidth, OrthoHeight, 1.0f / FarPlane, NearPlane);

	FMatrix OffsetMatrix = FTranslationMatrix(FVector(ShearOffset.X, ShearOffset.Y, 0.0f));

	return
		ShearMatrix
		* OrthoMatrix
		* OffsetMatrix;
}

void FOrthoHMD::InitCanvasFromView(FSceneView* InView, UCanvas* Canvas)
{
}

void FOrthoHMD::GetEyeRenderParams_RenderThread(const FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
{
	EyeToSrcUVOffsetValue = FVector2D::ZeroVector;
	EyeToSrcUVScaleValue = FVector2D(1.0f, 1.0f);
}


void FOrthoHMD::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	InViewFamily.EngineShowFlags.HMDDistortion = true;
	InViewFamily.EngineShowFlags.SetScreenPercentage(true);
	InViewFamily.EngineShowFlags.StereoRendering = IsStereoEnabled();
}

void FOrthoHMD::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	InView.BaseHmdOrientation = FQuat(FRotator(0.0f,0.0f,0.0f));
	InView.BaseHmdLocation = FVector(0.f);
	InViewFamily.bUseSeparateRenderTarget = false;

	UOrthoHMDDeveloperSettings* DeveloperSettings = StaticCast<UOrthoHMDDeveloperSettings*>(UOrthoHMDDeveloperSettings::StaticClass()->GetDefaultObject());

	ShearFactor = DeveloperSettings->InitialShearFactor;
	ShearOffset = DeveloperSettings->InitialShearOffset;
}

void FOrthoHMD::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
	check(IsInRenderingThread());
}

void FOrthoHMD::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	check(IsInRenderingThread());
}

FOrthoHMD::FOrthoHMD() :
	CurHmdOrientation(FQuat::Identity),
	LastHmdOrientation(FQuat::Identity),
	DeltaControlRotation(FRotator::ZeroRotator),
	DeltaControlOrientation(FQuat::Identity),
	LastSensorTime(-1.0)
{
}

FOrthoHMD::~FOrthoHMD()
{
}

bool FOrthoHMD::IsInitialized() const
{
	return true;
}
