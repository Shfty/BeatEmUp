// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "MovesetGraph/SMovesetGraphNode_Base.h"
#include "MovesetGraph/MovesetGraphNode_Move.h"
#include "SConstraintCanvas.h"
#include "SOverlay.h"

class FAssetThumbnail;
class FAssetThumbnailPool;
class SButton;
class SPaperFlipbook;
class SSlider;
class UPaperFlipbook;
class UFrameData;
struct FSlateImageBrush;

class SMovesetGraphNode_Move : public SMovesetGraphNode_Base
{
public:
	SLATE_BEGIN_ARGS(SMovesetGraphNode_Move) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UMovesetGraphNode_Move* InNode);

protected:
	// SGraphNode interface
	virtual TSharedRef<SWidget> CreateCenterContentArea() override;

private:
	const FSlateBrush* GetPlayStatusIcon() const;
	const TArray<UFrameData*> GetFrameDataForFrame(int32 Frame) const;

	FText HandleCurrentFrameText() const;
	FText HandleFrameDataText() const;
	void HandleSliderValueChanged(float NewValue);
	void HandleFlipbookPlaybackPositionChanged(float NewPlaybackPosition);

	FReply HandleGotoStartClicked();
	FReply HandlePrevFrameClicked();
	FReply HandlePlayPauseClicked();
	FReply HandleNextFrameClicked();
	FReply HandleGotoEndClicked();

	void SetFlipbookPlaying(bool NewPlaying);
	void SetFlipbookPlaybackPosition(float NewTime);

private:
	UMovesetGraphNode_Move* MovesetGraphNode;

	UPaperFlipbook* Flipbook;

	TMap<UFrameData*, SConstraintCanvas::FSlot*> FrameDataMap;

	TSharedPtr<SPaperFlipbook> FlipbookWidget;
	SConstraintCanvas::FSlot* FlipbookSlot;

	TSharedPtr<SSlider> SliderWidget;
	TSharedPtr<SButton> PlayButton;

	TSharedPtr<SConstraintCanvas> FrameDataCanvas;
	TArray<SConstraintCanvas::FSlot*> FrameDataCanvasSlots;

	TSharedPtr<FSlateBrush> ActiveFrameBrush;
};
