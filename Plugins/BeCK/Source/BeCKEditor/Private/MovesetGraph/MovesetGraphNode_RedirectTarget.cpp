// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
MovesetGraphNode_Root.cpp
=============================================================================*/

#include "BeCKEditor.h"

#include "MovesetGraph/MovesetGraphNode_RedirectTarget.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorSettings.h"

#define LOCTEXT_NAMESPACE "MovesetGraphNode_Wildcard"

UMovesetGraphNode_RedirectTarget::UMovesetGraphNode_RedirectTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCanRenameNode = true;
}

FLinearColor UMovesetGraphNode_RedirectTarget::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->PureFunctionCallNodeTitleColor;
}

#undef LOCTEXT_NAMESPACE
