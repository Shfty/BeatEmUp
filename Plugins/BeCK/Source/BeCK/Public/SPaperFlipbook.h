// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "SConstraintCanvas.h"

class FAssetThumbnail;
class FAssetThumbnailPool;
class SConstraintCanvas;
class SScaleBox;
class SImage;
class UPaperSprite;
class UPaperFlipbook;

DECLARE_DELEGATE(FFlipbookFinishedDelegate)

class BECK_API SPaperFlipbook : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPaperFlipbook)
		: _Flipbook((UPaperFlipbook*)nullptr)
		, _Playing(true)
		, _Looping(true)
		, _ReversePlayback(false)
		, _PlayRate(1.0f)
		, _StartTime(-1.0f)
		, _StartFrame(-1)
		, _ColorAndOpacity(FLinearColor::White)
		, _OnMouseButtonDown()
		, _OnPlaybackPositionChanged()
		, _OnPlaybackFinished()
		{}

		SLATE_ATTRIBUTE(UPaperFlipbook*, Flipbook)

		SLATE_ATTRIBUTE(bool, Playing)

		SLATE_ATTRIBUTE(bool, Looping)

		SLATE_ATTRIBUTE(bool, ReversePlayback)

		SLATE_ATTRIBUTE(float, PlayRate)

		SLATE_ATTRIBUTE(float, StartTime)

		SLATE_ATTRIBUTE(int32, StartFrame)

		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

		SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)

		SLATE_EVENT(FOnFloatValueChanged, OnPlaybackPositionChanged)

		SLATE_EVENT(FSimpleDelegate, OnPlaybackFinished)

	SLATE_END_ARGS()

	/** Constructor */
	SPaperFlipbook()
	{
		bCanTick = true;
		bCanSupportFocus = false;
	}

	void Construct(const FArguments& InArgs);

public:
	UPaperFlipbook* GetFlipbook() const { return Flipbook.Get(); };
	bool GetPlaying() const { return Playing.Get(); };
	bool GetLooping() const { return Looping.Get(); };
	bool GetReversePlayback() const { return ReversePlayback.Get(); };
	float GetPlayRate() const { return PlayRate.Get(); };

	void SetFlipbook(TAttribute<UPaperFlipbook*> InFlipbook);
	void SetPlaying(TAttribute<bool> InPlaying);
	void SetLooping(TAttribute<bool> InLooping);
	void SetReversePlayback(TAttribute<bool> InReversePlayback);
	void SetPlayRate(TAttribute<float> InReversePlayback);
	void SetStartTime(TAttribute<float> InStartTime);
	void SetStartFrame(TAttribute<int32> InStartFrame);
	void SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	void SetOnMouseButtonDown(FPointerEventHandler EventHandler);
	
	void Play();
	void PlayFromStart();
	void Reverse();
	void ReverseFromEnd();
	void Stop();

	float GetPlaybackPosition() const;
	int32 GetPlaybackPositionInFrames() const;
	float GetFlipbookLength() const;
	int32 GetFlipbookLengthInFrames() const;
	float GetFlipbookFramerate() const;
	void GetSlateTransformAtTime(float Time, FVector2D& Position, FVector2D& Size, FVector2D& Alignment) const;
	void GetSlateTransformForFrame(int32 Frame, FVector2D& Position, FVector2D& Size, FVector2D& Alignment) const;
	void GetSlateTransformForSprite(UPaperSprite* Sprite, FVector2D& Position, FVector2D& Size, FVector2D& Alignment) const;

	void SetPlaybackPosition(float NewPosition, bool bFireEvents);
	void SetPlaybackPositionInFrames(int32 NewFramePosition, bool bFireEvents);
	void SetNewTime(float NewTime);

public:
	// SWidget overrides
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:
	void UpdateBrush();
	void TickFlipbook(float DeltaTime);
	void CalculateCurrentFrame();
	FVector2D GetFlipbookOrigin() const;
	FVector2D GetFlipbookSize() const;

protected:
	/** The FName of the paper sprite to show */
	TAttribute<UPaperFlipbook*> Flipbook;

	TAttribute<bool> Playing;
	TAttribute<bool> Looping;
	TAttribute<bool> ReversePlayback;
	TAttribute<float> PlayRate;
	TAttribute<int32> StartFrame;
	TAttribute<float> StartTime;

	float AccumulatedTime;
	int32 CachedFrameIndex;

	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

	/** Invoked when the mouse is pressed in the image */
	FPointerEventHandler OnMouseButtonDownHandler;

	// Invoked when the current 
	FOnFloatValueChanged OnPlaybackPositionChanged;

	// Invoked when the flipbook reaches its end
	FFlipbookFinishedDelegate OnPlaybackFinished;

	TSharedPtr<SConstraintCanvas> ConstraintCanvas;
	SConstraintCanvas::FSlot* CanvasSlot;
	TSharedPtr<SImage> Image;
	TSharedPtr<FSlateBrush> SpriteBrush;
};
