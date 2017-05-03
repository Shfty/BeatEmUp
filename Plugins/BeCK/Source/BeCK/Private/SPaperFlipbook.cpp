// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeCK.h"

#include "SPaperFlipbook.h"
#include "SImage.h"
#include "SScaleBox.h"
#include "SConstraintCanvas.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"

void SPaperFlipbook::Construct(const FArguments& InArgs)
{
	Flipbook = InArgs._Flipbook;
	Playing = InArgs._Playing;
	Looping = InArgs._Looping;
	ReversePlayback = InArgs._ReversePlayback;
	PlayRate = InArgs._PlayRate;
	StartTime = InArgs._StartTime;
	StartFrame = InArgs._StartFrame;
	AccumulatedTime = 0.0f;
	CachedFrameIndex = 0;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	OnPlaybackPositionChanged = InArgs._OnPlaybackPositionChanged;
	OnPlaybackFinished = InArgs._OnPlaybackFinished;
	OnMouseButtonDownHandler = InArgs._OnMouseButtonDown;

	SpriteBrush = MakeShareable(new FSlateBrush());

	ChildSlot
	[
		SAssignNew(ConstraintCanvas, SConstraintCanvas)
	];

	ConstraintCanvas->AddSlot()
	.Anchors(FAnchors(0.5f, 0.5f))
	.AutoSize(false)
	.Expose(CanvasSlot)
	[
		SAssignNew(Image, SImage)
		.Image(SpriteBrush.Get())
	];

	float StartTimeValue = StartTime.Get();
	if(StartTimeValue != -1.0f)
	{
		SetPlaybackPosition(StartTimeValue, true);
	}

	int32 StartFrameValue = StartFrame.Get();
	if(StartFrameValue != -1)
	{
		SetPlaybackPositionInFrames(StartFrameValue, true);
	}

	UpdateBrush();
}

float SPaperFlipbook::GetPlaybackPosition() const
{
	return AccumulatedTime;
}

int32 SPaperFlipbook::GetPlaybackPositionInFrames() const
{
	const float Framerate = GetFlipbookFramerate();
	const int32 NumFrames = GetFlipbookLengthInFrames() - 1;
	if(NumFrames > 0)
	{
		return FMath::FloorToInt((AccumulatedTime / GetFlipbookLength()) * NumFrames);
	}
	else
	{
		return 0;
	}
}

float SPaperFlipbook::GetFlipbookLength() const
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		return PaperFlipbook->GetTotalDuration();
	}

	return 0.0f;
}

int32 SPaperFlipbook::GetFlipbookLengthInFrames() const
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		return PaperFlipbook->GetNumFrames();
	}

	return 0;
}

float SPaperFlipbook::GetFlipbookFramerate() const
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		return PaperFlipbook->GetFramesPerSecond();
	}

	return 15.0f;
}

void SPaperFlipbook::SetNewTime(float NewTime)
{
	SetPlaybackPosition(NewTime, false);
}

void SPaperFlipbook::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{

	TickFlipbook(InDeltaTime);
	CalculateCurrentFrame();
}

/**
* The system calls this method to notify the widget that a mouse button was pressed within it. This event is bubbled.
*
* @param MyGeometry The Geometry of the widget receiving the event
* @param MouseEvent Information about the input event
*
* @return Whether the event was handled along with possible requests for the system to take action.
*/
FReply SPaperFlipbook::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(OnMouseButtonDownHandler.IsBound())
	{
		// If a handler is assigned, call it.
		return OnMouseButtonDownHandler.Execute(MyGeometry, MouseEvent);
	}
	else
	{
		// otherwise the event is unhandled.
		return FReply::Unhandled();
	}
}

/**
* An Image's desired size is whatever the image looks best as. This is decided on a case by case basis via the Width and Height properties.
*/
FVector2D SPaperFlipbook::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return GetFlipbookSize();
}

void SPaperFlipbook::UpdateBrush()
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		UPaperSprite* Sprite = PaperFlipbook->GetSpriteAtFrame(CachedFrameIndex);
		if(Sprite != nullptr)
		{
			SpriteBrush->SetResourceObject(Sprite);

			FVector2D Position;
			FVector2D Size;
			FVector2D Alignment;

#if WITH_EDITOR
			GetSlateTransformForFrame(CachedFrameIndex, Position, Size, Alignment);
#endif

			CanvasSlot->Offset(FMargin(Position.X, Position.Y, Size.X, Size.Y));
			CanvasSlot->Alignment(Alignment);
		}
	}
}

void SPaperFlipbook::TickFlipbook(float DeltaTime)
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		bool bIsFinished = false;

		if(GetPlaying())
		{
			const float TimelineLength = PaperFlipbook->GetTotalDuration();
			float PlayRateValue = GetPlayRate();
			const float EffectiveDeltaTime = DeltaTime * (GetReversePlayback() ? -PlayRateValue : PlayRateValue);

			float NewPosition = AccumulatedTime + EffectiveDeltaTime;

			if(EffectiveDeltaTime > 0.0f)
			{
				if(NewPosition > TimelineLength)
				{
					if(GetLooping())
					{
						// If looping, play to end, jump to start, and set target to somewhere near the beginning.
						SetPlaybackPosition(TimelineLength, true);
						SetPlaybackPosition(0.0f, false);

						if(TimelineLength > 0.0f)
						{
							while(NewPosition > TimelineLength)
							{
								NewPosition -= TimelineLength;
							}
						}
						else
						{
							NewPosition = 0.0f;
						}
					}
					else
					{
						// If not looping, snap to end and stop playing.
						NewPosition = TimelineLength;
						Stop();
						bIsFinished = true;
					}
				}
			}
			else
			{
				if(NewPosition < 0.0f)
				{
					if(GetLooping())
					{
						// If looping, play to start, jump to end, and set target to somewhere near the end.
						SetPlaybackPosition(0.0f, true);
						SetPlaybackPosition(TimelineLength, false);

						if(TimelineLength > 0.0f)
						{
							while(NewPosition < 0.0f)
							{
								NewPosition += TimelineLength;
							}
						}
						else
						{
							NewPosition = 0.0f;
						}
					}
					else
					{
						// If not looping, snap to start and stop playing.
						NewPosition = 0.0f;
						Stop();
						bIsFinished = true;
					}
				}
			}

			SetPlaybackPosition(NewPosition, true);
		}

		// Notify user that the flipbook finished playing
		if(bIsFinished)
		{
			OnPlaybackFinished.ExecuteIfBound();
		}
	}
}

void SPaperFlipbook::CalculateCurrentFrame()
{
	const int32 LastCachedFrame = CachedFrameIndex;
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	CachedFrameIndex = (PaperFlipbook != nullptr) ? PaperFlipbook->GetKeyFrameIndexAtTime(AccumulatedTime) : INDEX_NONE;

	if(CachedFrameIndex != LastCachedFrame)
	{
		UpdateBrush();
	}
}

FVector2D SPaperFlipbook::GetFlipbookOrigin() const
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		FVector FlipbookOrigin = PaperFlipbook->GetRenderBounds().Origin;
		return FVector2D(FlipbookOrigin.X, FlipbookOrigin.Z);
	}

	return FVector2D::ZeroVector;
}

FVector2D SPaperFlipbook::GetFlipbookSize() const
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		FVector FlipbookBounds = PaperFlipbook->GetRenderBounds().BoxExtent;
		return FVector2D(FlipbookBounds.X * 2.0f, FlipbookBounds.Z * 2.0f);
	}

	return FVector2D::ZeroVector;
}

#if WITH_EDITOR
void SPaperFlipbook::GetSlateTransformAtTime(float Time, FVector2D& Position, FVector2D& Size, FVector2D& Alignment) const
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		GetSlateTransformForSprite(PaperFlipbook->GetSpriteAtTime(Time), Position, Size, Alignment);
	}
}

void SPaperFlipbook::GetSlateTransformForFrame(int32 Frame, FVector2D& Position, FVector2D& Size, FVector2D& Alignment) const
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook != nullptr)
	{
		GetSlateTransformForSprite(PaperFlipbook->GetSpriteAtFrame(Frame), Position, Size, Alignment);
	}
}

void SPaperFlipbook::GetSlateTransformForSprite(UPaperSprite* Sprite, FVector2D& Position, FVector2D& Size, FVector2D& Alignment) const
{
	if(Sprite != nullptr)
	{
		FVector2D PivotPosition = Sprite->GetPivotPosition();
		FVector2D SourceUV = Sprite->GetSourceUV();
		FVector2D SourceDimension = Sprite->GetSourceSize();

		FVector2D LocalPivot = PivotPosition - SourceUV;
		FVector2D ScalarPivot = LocalPivot / SourceDimension;

		FVector2D FlipbookOrigin = GetFlipbookOrigin();

		Position = FVector2D(-FlipbookOrigin.X, FlipbookOrigin.Y);
		Size = SourceDimension;
		Alignment = ScalarPivot;
	}
}
#endif

void SPaperFlipbook::SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity)
{
	Image->SetColorAndOpacity(InColorAndOpacity);
}

void SPaperFlipbook::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	Image->SetColorAndOpacity(InColorAndOpacity);
}

void SPaperFlipbook::SetFlipbook(TAttribute<UPaperFlipbook*> InFlipbook)
{
	Flipbook = InFlipbook;
	UpdateBrush();
}

void SPaperFlipbook::SetOnMouseButtonDown(FPointerEventHandler EventHandler)
{
	OnMouseButtonDownHandler = EventHandler;
}

void SPaperFlipbook::SetPlaybackPosition(float NewPosition, bool bFireEvents)
{
	UPaperFlipbook* SourceFlipbook = GetFlipbook();
	if(SourceFlipbook)
	{
		float OldPosition = AccumulatedTime;
		AccumulatedTime = NewPosition;

		// If we should be firing events for this track...
		if(bFireEvents)
		{
			float MinTime;
			float MaxTime;
			if(!GetReversePlayback())
			{
				// If playing sequence forwards.
				MinTime = OldPosition;
				MaxTime = AccumulatedTime;

				// Slight hack here.. if playing forwards and reaching the end of the sequence, force it over a little to ensure we fire events actually on the end of the sequence.
				if(MaxTime == SourceFlipbook->GetTotalDuration())
				{
					MaxTime += (float)KINDA_SMALL_NUMBER;
				}
			}
			else
			{
				// If playing sequence backwards.
				MinTime = AccumulatedTime;
				MaxTime = OldPosition;

				// Same small hack as above for backwards case.
				if(MinTime == 0.0f)
				{
					MinTime -= (float)KINDA_SMALL_NUMBER;
				}
			}

#if 0
			// See which events fall into traversed region.
			for(int32 i = 0; i < Events.Num(); i++)
			{
				float EventTime = Events[i].Time;

				// Need to be slightly careful here and make behavior for firing events symmetric when playing forwards of backwards.
				bool bFireThisEvent = false;
				if(!bReversePlayback)
				{
					if((EventTime >= MinTime) && (EventTime < MaxTime))
					{
						bFireThisEvent = true;
					}
				}
				else
				{
					if((EventTime > MinTime) && (EventTime <= MaxTime))
					{
						bFireThisEvent = true;
					}
				}

				if(bFireThisEvent)
				{
					Events[i].EventFunc.ExecuteIfBound();
				}
			}
#endif
		}

		OnPlaybackPositionChanged.ExecuteIfBound(AccumulatedTime);

		if(OldPosition != AccumulatedTime)
		{
			CalculateCurrentFrame();
		}
	}
}

void SPaperFlipbook::SetPlaybackPositionInFrames(int32 NewFramePosition, bool bFireEvents)
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook)
	{
		const float Framerate = PaperFlipbook->GetFramesPerSecond();
		const float NewTime = (Framerate > 0.0f) ? (NewFramePosition / Framerate) : 0.0f;
		SetPlaybackPosition(NewTime, bFireEvents);
	}
}

void SPaperFlipbook::Play()
{
	SetPlaying(true);
}

void SPaperFlipbook::Stop()
{
	SetPlaying(false);
}

void SPaperFlipbook::ReverseFromEnd()
{
	UPaperFlipbook* PaperFlipbook = GetFlipbook();
	if(PaperFlipbook)
	{
		SetPlaybackPosition(PaperFlipbook->GetTotalDuration(), /*bFireEvents=*/ false);
		Reverse();
	}
}

void SPaperFlipbook::Reverse()
{
	SetReversePlayback(false);
	SetPlaying(true);
}

void SPaperFlipbook::PlayFromStart()
{
	SetPlaybackPosition(0.0f, /*bFireEvents=*/ false);
	SetReversePlayback(false);
	SetPlaying(true);
}

void SPaperFlipbook::SetPlaying(TAttribute<bool> InPlaying)
{
	Playing = InPlaying;
}

void SPaperFlipbook::SetLooping(TAttribute<bool> InLooping)
{
	Looping = InLooping;
}

void SPaperFlipbook::SetReversePlayback(TAttribute<bool> InReversePlayback)
{
	ReversePlayback = InReversePlayback;
}

void SPaperFlipbook::SetPlayRate(TAttribute<float> InPlayRate)
{
	PlayRate = InPlayRate;
}

void SPaperFlipbook::SetStartTime(TAttribute<float> InStartTime)
{
	StartTime = InStartTime;
}

void SPaperFlipbook::SetStartFrame(TAttribute<int32> InStartFrame)
{
	StartFrame = InStartFrame;
}
