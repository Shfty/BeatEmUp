// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUp.h"

#include "MovesetNode_FrameData.h"
#include "Move.h"

UMovesetNode_FrameData::UMovesetNode_FrameData(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

#if WITH_EDITOR

FText UMovesetNode_FrameData::GetTitle() const
{
	if(FrameData != nullptr)
	{
		return FrameData->GetClass()->GetDisplayNameText();
	}

	return FText::FromString(TEXT("Frame Data"));
}

#endif
