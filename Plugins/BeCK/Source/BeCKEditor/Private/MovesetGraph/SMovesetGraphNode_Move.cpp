// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeCKEditor.h"

#include "SMovesetGraphNode_Base.h"
#include "SMovesetGraphNode_Move.h"
#include "Widgets/SBoxPanel.h"
#include "SBox.h"
#include "SScaleBox.h"
#include "SImage.h"
#include "SButton.h"
#include "SSlider.h"
#include "SOverlay.h"
#include "SPaperFlipbook.h"
#include "SlateImageBrush.h"
#include "DrawElements.h"
#include "AssetThumbnail.h"
#include "PaperFlipbook.h"
#include "MovesetNode_Move.h"
#include "MovesetNode_FrameData.h"
#include "Move.h"
#include "Curves/CurveVector.h"

void SMovesetGraphNode_Move::Construct(const FArguments& InArgs, UMovesetGraphNode_Move* InNode)
{
	this->GraphNode = Cast<UEdGraphNode>(InNode);
	this->MovesetNode = InNode;

	ActiveFrameBrush = MakeShareable(new FSlateBrush());
	ActiveFrameBrush->TintColor = FSlateColor(FLinearColor::Red);

	this->UpdateGraphNode();
}

TSharedRef<SWidget> SMovesetGraphNode_Move::CreateCenterContentArea()
{
	FrameDataCanvas = SNew(SConstraintCanvas);

	MovesetGraphNode = Cast<UMovesetGraphNode_Move>(GetNodeObj());
	if(MovesetGraphNode != nullptr)
	{
		UMovesetNode_Move* MoveNode = Cast<UMovesetNode_Move>(MovesetGraphNode->MovesetNode);
		if(MoveNode != nullptr)
		{
			UMove* Move = MoveNode->Move;
			if(Move != nullptr)
			{
				Flipbook = Move->Flipbook;
			}

			for(UMovesetNode_Base* Node : MoveNode->ChildNodes)
			{
				UMovesetNode_FrameData* FrameDataNode = Cast<UMovesetNode_FrameData>(Node);
				if(FrameDataNode != nullptr && FrameDataNode->FrameData != nullptr)
				{

					SConstraintCanvas::FSlot* CanvasSlot;
					FrameDataCanvas->AddSlot()
						.AutoSize(false)
						.Anchors(FAnchors(0.5f, 0.5f))
						.Alignment(FVector2D(0.0f, 0.0f))
						.Expose(CanvasSlot)
						[
							SNew(SImage)
							.ColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 0.5f))
						];

					FrameDataMap.Add(FrameDataNode->FrameData, CanvasSlot);
				}
			}
		}
	}

	SliderWidget = SNew(SSlider)
		.OnValueChanged(this, &SMovesetGraphNode_Move::HandleSliderValueChanged);

	if(Flipbook != nullptr)
	{
		SliderWidget->SetStepSize(1.0f / Flipbook->GetFramesPerSecond());
	}

	return SNew(SVerticalBox)
	+SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryTop"))
		.Padding(FMargin(6.0f))
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SConstraintCanvas)
				+SConstraintCanvas::Slot()
				.AutoSize(true)
				.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
				.Expose(FlipbookSlot)
				[
					SNew(SBox)
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SOverlay)
						+ SOverlay::Slot()
						[
							SAssignNew(FlipbookWidget, SPaperFlipbook)
							.Flipbook(Flipbook)
							.Playing(MovesetGraphNode->FlipbookPlaying)
							.StartTime(MovesetGraphNode->FlipbookPlaybackPosition)
							.OnPlaybackPositionChanged(this, &SMovesetGraphNode_Move::HandleFlipbookPlaybackPositionChanged)
						]
						+SOverlay::Slot()
						[
							FrameDataCanvas.ToSharedRef()
						]
					]
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(EVerticalAlignment::VAlign_Bottom)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				[
					SNew(STextBlock)
					.Text(this, &SMovesetGraphNode_Move::HandleCurrentFrameText)
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				[
					SNew(STextBlock)
					.Text(this, &SMovesetGraphNode_Move::HandleFrameDataText)
				]
			]
		]
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryMiddle"))
		[
			SliderWidget.ToSharedRef()
		]
	]
	+SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryBottom"))
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Backward_End"))
				.OnClicked(this, &SMovesetGraphNode_Move::HandleGotoStartClicked)
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Backward_Step"))
				.OnClicked(this, &SMovesetGraphNode_Move::HandlePrevFrameClicked)
			]
			+ SHorizontalBox::Slot()
			[
				SAssignNew(PlayButton, SButton)
				.ButtonStyle(FEditorStyle::Get(), "NoBorder")
				.OnClicked(this, &SMovesetGraphNode_Move::HandlePlayPauseClicked)
				[
					SNew(SImage)
					.Image(this, &SMovesetGraphNode_Move::GetPlayStatusIcon)
				]
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Forward_Step"))
				.OnClicked(this, &SMovesetGraphNode_Move::HandleNextFrameClicked)
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Forward_End"))
				.OnClicked(this, &SMovesetGraphNode_Move::HandleGotoEndClicked)
			]
		]
	];
}

const FSlateBrush* SMovesetGraphNode_Move::GetPlayStatusIcon() const
{
	if(FlipbookWidget->GetPlaying())
	{
		return PlayButton.IsValid() && PlayButton->IsPressed() ?
			&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Pause").Pressed :
			&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Pause").Normal;
	}

	return PlayButton.IsValid() && PlayButton->IsPressed() ?
		&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Forward").Pressed :
		&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Forward").Normal;
}

const TArray<UFrameData*> SMovesetGraphNode_Move::GetFrameDataForFrame(int32 Frame) const
{
	TArray<UFrameData*> FrameDataArray;

	for(TPair<UFrameData*, SConstraintCanvas::FSlot*> FrameDataPair : FrameDataMap)
	{
		UFrameData* FrameData = FrameDataPair.Key;
		SConstraintCanvas::FSlot* CanvasSlot = FrameDataPair.Value;

		if(FrameData != nullptr)
		{
			if(Frame >= FrameData->ActivePeriod.Min && Frame <= FrameData->ActivePeriod.Max)
			{
				FrameDataArray.Add(FrameData);
			}
		}
	}

	return FrameDataArray;
}

FText SMovesetGraphNode_Move::HandleCurrentFrameText() const
{
	if(FlipbookWidget.IsValid())
	{
		int32 CurrentFrame = FlipbookWidget->GetPlaybackPositionInFrames();
		return FText::FromString(FString::Printf(TEXT("Frame %d"), CurrentFrame));
	}
	return FText::GetEmpty();
}

FText SMovesetGraphNode_Move::HandleFrameDataText() const
{
	if(FlipbookWidget.IsValid())
	{
		FString OutString;

		const TArray<UFrameData*> FrameDataArray = GetFrameDataForFrame(FlipbookWidget->GetPlaybackPositionInFrames());
		for(UFrameData* FrameData : FrameDataArray)
		{
			UHitboxFrames* HitboxFrames = Cast<UHitboxFrames>(FrameData);
			if(HitboxFrames != nullptr)
			{
				OutString.Append(FString::Printf(TEXT("H%d / D%d"), HitboxFrames->HitIndex, HitboxFrames->Damage));
			}

			UArmorFrames* ArmorFrames = Cast<UArmorFrames>(FrameData);
			if(ArmorFrames != nullptr)
			{
				OutString.Append(FString::Printf(TEXT("A /"), *FString::SanitizeFloat(ArmorFrames->DamageMultiplier)));

				if(ArmorFrames->HasHitLimit)
				{
					OutString.Append(FString::Printf(TEXT(" H%d"), ArmorFrames->HitLimit));
				}

				if(ArmorFrames->HasDamageLimit)
				{
					OutString.Append(FString::Printf(TEXT(" D%d"), ArmorFrames->DamageLimit));
				}
			}

			UInvincibilityFrames* InvincibilityFrames = Cast<UInvincibilityFrames>(FrameData);
			if(InvincibilityFrames != nullptr)
			{
				OutString.Append(TEXT("I"));
			}

			ULinkFrames* LinkFrames = Cast<ULinkFrames>(FrameData);
			if(LinkFrames != nullptr)
			{
				OutString.Append(FString::Printf(TEXT("L%d"), LinkFrames->LinkIndex));
			}
		}
		return FText::FromString(OutString);
	}

	return FText::FromString(TEXT(" "));
}

void SMovesetGraphNode_Move::HandleSliderValueChanged(float NewValue)
{
	if(FlipbookWidget.IsValid())
	{
		if(!FlipbookWidget->GetPlaying())
		{
			FlipbookWidget->SetPlaybackPosition(NewValue * FlipbookWidget->GetFlipbookLength(), true);
		}
	}
}

void SMovesetGraphNode_Move::HandleFlipbookPlaybackPositionChanged(float NewPlaybackPosition)
{
	if(SliderWidget.IsValid() && FlipbookWidget.IsValid())
	{
		SliderWidget->SetValue(NewPlaybackPosition / FlipbookWidget->GetFlipbookLength());

		FVector2D Position;
		FVector2D Size;
		FVector2D Alignment;
		FlipbookWidget->GetSlateTransformAtTime(NewPlaybackPosition, Position, Size, Alignment);

		SetFlipbookPlaybackPosition(NewPlaybackPosition);

		const TArray<UFrameData*> FrameDataArray = GetFrameDataForFrame(FlipbookWidget->GetPlaybackPositionInFrames());

		// Update flipbook based on frame data
		if(FlipbookSlot != nullptr)
		{
			FlipbookSlot->Offset(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
		}
		FLinearColor AccumulatedColor = FLinearColor::White;

		for(UFrameData* FrameData : FrameDataArray)
		{
			UArmorFrames* ArmorFrames = Cast<UArmorFrames>(FrameData);
			if(ArmorFrames != nullptr)
			{
				AccumulatedColor *= FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
			}

			UInvincibilityFrames* InvincibilityFrames = Cast<UInvincibilityFrames>(FrameData);
			if(InvincibilityFrames != nullptr)
			{
				AccumulatedColor *= FLinearColor(1.0f, 1.0f, 1.0f, 0.5f);
			}

			ULinkFrames* LinkFrames = Cast<ULinkFrames>(FrameData);
			if(LinkFrames != nullptr)
			{
				AccumulatedColor *= FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
			}

			if(FlipbookSlot != nullptr)
			{
				UMotionFrames* MotionFrames = Cast<UMotionFrames>(FrameData);
				if(MotionFrames != nullptr)
				{
					UCurveVector* MotionCurve = MotionFrames->MotionCurve;
					if(MotionCurve != nullptr)
					{
						FVector MotionVector = MotionCurve->GetVectorValue(NewPlaybackPosition * FlipbookWidget->GetFlipbookFramerate());
						FlipbookSlot->Offset(FMargin(MotionVector.X, -MotionVector.Y, 0.0f, 0.0f));
					}
				}
			}
		}

		FlipbookWidget->SetColorAndOpacity(AccumulatedColor);

		// Update hitbox overlays
		for(TPair<UFrameData*, SConstraintCanvas::FSlot*> FrameDataPair : FrameDataMap)
		{
			UFrameData* FrameData = FrameDataPair.Key;
			SConstraintCanvas::FSlot* CanvasSlot = FrameDataPair.Value;
			if(FrameData != nullptr && CanvasSlot != nullptr)
			{
				FInt32Interval ActivePeriod = FrameData->ActivePeriod;
				if(FrameDataArray.Contains(FrameData))
				{
					UHitboxFrames* ActiveFrames = Cast<UHitboxFrames>(FrameData);
					if(ActiveFrames != nullptr)
					{
						FVector2D HitboxLocation = ActiveFrames->Location;
						FVector2D HitboxSize = ActiveFrames->Size;

						CanvasSlot->GetWidget()->SetVisibility(EVisibility::Visible);
						CanvasSlot->Offset(FMargin(Position.X + HitboxLocation.X, Position.Y + HitboxLocation.Y, HitboxSize.X, HitboxSize.Y));
					}
				}
				else
				{
					CanvasSlot->GetWidget()->SetVisibility(EVisibility::Hidden);
				}
			}
		}
	}
}

FReply SMovesetGraphNode_Move::HandleGotoStartClicked()
{
	if(FlipbookWidget.IsValid())
	{
		SetFlipbookPlaying(false);
		FlipbookWidget->SetPlaybackPosition(0.0f, true);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SMovesetGraphNode_Move::HandlePrevFrameClicked()
{
	if(FlipbookWidget.IsValid())
	{
		SetFlipbookPlaying(false);
		float NewPlaybackPosition = FMath::Max(0.0f, FlipbookWidget->GetPlaybackPosition() - (1.0f / FlipbookWidget->GetFlipbookFramerate()));
		FlipbookWidget->SetPlaybackPosition(NewPlaybackPosition, true);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SMovesetGraphNode_Move::HandlePlayPauseClicked()
{
	if(FlipbookWidget.IsValid())
	{
		SetFlipbookPlaying(!MovesetGraphNode->FlipbookPlaying);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SMovesetGraphNode_Move::HandleNextFrameClicked()
{
	if(FlipbookWidget.IsValid())
	{
		SetFlipbookPlaying(false);
		float NewPlaybackPosition = FMath::Min(FlipbookWidget->GetFlipbookLength(), FlipbookWidget->GetPlaybackPosition() + (1.0f / FlipbookWidget->GetFlipbookFramerate()));
		FlipbookWidget->SetPlaybackPosition(NewPlaybackPosition, true);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SMovesetGraphNode_Move::HandleGotoEndClicked()
{
	if(FlipbookWidget.IsValid())
	{
		SetFlipbookPlaying(false);
		FlipbookWidget->SetPlaybackPosition(FlipbookWidget->GetFlipbookLength(), true);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SMovesetGraphNode_Move::SetFlipbookPlaying(bool NewPlaying)
{
	MovesetGraphNode->FlipbookPlaying = NewPlaying;

	if(FlipbookWidget.IsValid())
	{
		FlipbookWidget->SetPlaying(NewPlaying);
	}
}

void SMovesetGraphNode_Move::SetFlipbookPlaybackPosition(float NewTime)
{
	MovesetGraphNode->FlipbookPlaybackPosition = NewTime;

	if(FlipbookWidget.IsValid())
	{
		FlipbookWidget->SetStartTime(NewTime);
	}
}
