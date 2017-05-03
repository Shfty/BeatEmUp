// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeCK.h"
#include "PaperFlipbookWidget.h"
#include "Slate/SlateBrushAsset.h"
#include "Materials/MaterialInterface.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SPaperFlipbook.h"
#include "PaperFlipbook.h"

#define LOCTEXT_NAMESPACE "UMG"

UPaperFlipbookWidget::UPaperFlipbookWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Playing(true)
	, Looping(true)
	, ReversePlayback(false)
	, StartingFrame(0)
	, PlayRate(1.0f)
	, ColorAndOpacity(FLinearColor::White)
{

}

void UPaperFlipbookWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyFlipbook.Reset();
}

TSharedRef<SWidget> UPaperFlipbookWidget::RebuildWidget()
{
	MyFlipbook = SNew(SPaperFlipbook);
	return MyFlipbook.ToSharedRef();
}

void UPaperFlipbookWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FSlateColor> ColorAndOpacityBinding = OPTIONAL_BINDING(FSlateColor, ColorAndOpacity);

	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetFlipbook(Flipbook);
		MyFlipbook->SetPlaying(Playing);
		MyFlipbook->SetLooping(Looping);
		MyFlipbook->SetReversePlayback(ReversePlayback);
		MyFlipbook->SetPlayRate(PlayRate);

		StartingFrame = FMath::Clamp(StartingFrame, 0, Flipbook->GetNumFrames());
		MyFlipbook->SetPlaybackPositionInFrames(StartingFrame, true);

		MyFlipbook->SetColorAndOpacity(ColorAndOpacityBinding);
		MyFlipbook->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
	}
}

void UPaperFlipbookWidget::SetFlipbook(UPaperFlipbook* InFlipbook)
{
	Flipbook = InFlipbook;

	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetFlipbook(Flipbook);
	}
}

void UPaperFlipbookWidget::SetPlaying(bool InPlaying)
{
	Playing = InPlaying;

	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetPlaying(Playing);
	}
}

void UPaperFlipbookWidget::SetLooping(bool InLooping)
{
	Looping = InLooping;

	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetLooping(Looping);
	}
}

void UPaperFlipbookWidget::SetReversePlayback(bool InReversePlayback)
{
	ReversePlayback = InReversePlayback;

	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetReversePlayback(ReversePlayback);
	}
}

void UPaperFlipbookWidget::SetPlayRate(float InPlayRate)
{
	PlayRate = InPlayRate;

	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetPlayRate(PlayRate);
	}
}

void UPaperFlipbookWidget::SetStartingFrame(int InStartingFrame)
{
	StartingFrame = FMath::Clamp(InStartingFrame, 0, Flipbook->GetNumFrames());

	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetPlaybackPositionInFrames(StartingFrame, true);
	}
}

void UPaperFlipbookWidget::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetColorAndOpacity(ColorAndOpacity);
	}
}

void UPaperFlipbookWidget::SetOpacity(float InOpacity)
{
	ColorAndOpacity.A = InOpacity;
	if(MyFlipbook.IsValid())
	{
		MyFlipbook->SetColorAndOpacity(ColorAndOpacity);
	}
}

FReply UPaperFlipbookWidget::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(OnMouseButtonDownEvent.IsBound())
	{
		return OnMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

#if WITH_EDITOR

const FText UPaperFlipbookWidget::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

#undef LOCTEXT_NAMESPACE
