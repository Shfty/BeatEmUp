// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
MovesetGraphNode_Root.cpp
=============================================================================*/

#include "BeCKEditor.h"

#include "MovesetGraph/MovesetGraphNode_Wildcard.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorSettings.h"
#include "MovesetGraphSchema.h"

#define LOCTEXT_NAMESPACE "MovesetGraphNode_Wildcard"

UMovesetGraphNode_Wildcard::UMovesetGraphNode_Wildcard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLinearColor UMovesetGraphNode_Wildcard::GetNodeTitleColor() const
{
	return FLinearColor::Blue;
}

FText UMovesetGraphNode_Wildcard::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("WildcardTitle", "Any");
}

FText UMovesetGraphNode_Wildcard::GetTooltipText() const
{
	return LOCTEXT("WildcardToolTip", "Wildcard, link to input nodes that should trigger regardless of current move");
}

void UMovesetGraphNode_Wildcard::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	if(Context.Pin)
	{
	}
	else if(Context.Node)
	{
	}
}

void UMovesetGraphNode_Wildcard::CreateOutputPins()
{
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, TEXT(""), nullptr, false, false, CreateUniquePinName(TEXT("Update")));
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, TEXT(""), nullptr, false, false, CreateUniquePinName(TEXT("Finish")));
}

#undef LOCTEXT_NAMESPACE
