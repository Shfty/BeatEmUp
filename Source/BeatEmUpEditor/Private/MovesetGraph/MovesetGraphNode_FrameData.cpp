// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpEditor.h"
#include "MovesetGraphNode_FrameData.h"
#include "MovesetGraph/MovesetGraphSchema.h"

#define LOCTEXT_NAMESPACE "MovesetGraphNode_Redirect"

UMovesetGraphNode_FrameData::UMovesetGraphNode_FrameData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMovesetGraphNode_FrameData::CreateOutputPins()
{
	UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UMovesetGraphSchema::PC_FrameData, TEXT(""), nullptr, false, false, TEXT("Output"));
	OutputPin->PinFriendlyName = FText::FromString(TEXT(" "));
}

#undef LOCTEXT_NAMESPACE
