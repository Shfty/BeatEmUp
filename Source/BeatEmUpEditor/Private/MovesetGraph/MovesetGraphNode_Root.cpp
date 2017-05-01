// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
MovesetGraphNode_Root.cpp
=============================================================================*/

#include "BeatEmUpEditor.h"

#include "MovesetGraph/MovesetGraphNode_Root.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorSettings.h"
#include "MovesetGraphSchema.h"
//#include "MovesetGraphEditorCommands.h"

#define LOCTEXT_NAMESPACE "MovesetGraphNode_Root"

UMovesetGraphNode_Root::UMovesetGraphNode_Root(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCanRenameNode = false;
	CustomTitle = FName(TEXT("Entry Point"));
}

FText UMovesetGraphNode_Root::GetTooltipText() const
{
	return LOCTEXT("RootToolTip", "Entry point, defines the moveset's initial state");
}

#undef LOCTEXT_NAMESPACE
