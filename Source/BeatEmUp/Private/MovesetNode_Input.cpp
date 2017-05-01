// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUp.h"

#include "MovesetNode_Input.h"
#include "Move.h"

UMovesetNode_Input::UMovesetNode_Input(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

#if WITH_EDITOR

void UMovesetNode_Input::SetPressedNode(UMovesetNode_Base* InPressedNode)
{
	PressedNode = InPressedNode;
}

void UMovesetNode_Input::SetReleasedNode(UMovesetNode_Base* InReleasedNode)
{
	ReleasedNode = InReleasedNode;
}

void UMovesetNode_Input::SetTrueNode(UMovesetNode_Base* InTrueNode)
{
	TrueNode = InTrueNode;
}

void UMovesetNode_Input::SetFalseNode(UMovesetNode_Base* InFalseNode)
{
	FalseNode = InFalseNode;
}

FText UMovesetNode_Input::GetTitle() const
{
	return FText::FromString(TEXT("Input"));
}

#endif
