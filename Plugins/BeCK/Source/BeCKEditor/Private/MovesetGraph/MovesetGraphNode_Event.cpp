// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BeCKEditor.h"
#include "MovesetGraphNode_Event.h"

#define LOCTEXT_NAMESPACE "MovesetGraphNode_Event"

UMovesetGraphNode_Event::UMovesetGraphNode_Event(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FLinearColor UMovesetGraphNode_Event::GetNodeTitleColor() const
{
	return FLinearColor::Yellow;
}

#undef LOCTEXT_NAMESPACE
