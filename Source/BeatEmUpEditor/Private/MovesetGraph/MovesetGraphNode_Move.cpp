// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
MovesetGraphNode_Move.cpp
=============================================================================*/

#include "BeatEmUpEditor.h"

#include "MovesetGraph/MovesetGraphNode_Move.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/EditorEngine.h"
#include "GraphEditorActions.h"
//#include "MovesetGraphEditorCommands.h"
#include "ScopedTransaction.h"
#include "Framework/Commands/GenericCommands.h"
#include "Engine/Font.h"
#include "MovesetGraph/MovesetGraph.h"
#include "MovesetGraph/MovesetGraphSchema.h"
#include "Moveset.h"
#include "MovesetNode_Move.h"
#include "MovesetNode_FrameData.h"
#include "FrameData.h"

/////////////////////////////////////////////////////
// UMovesetGraphNode_Move

UMovesetGraphNode_Move::UMovesetGraphNode_Move(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	FlipbookPlaying(false)
{
	bCanRenameNode = true;
}

FLinearColor UMovesetGraphNode_Move::GetNodeTitleColor() const
{
	return FLinearColor::Blue;
}

FText UMovesetGraphNode_Move::GetTooltipText() const
{
	FText Tooltip;
	if(MovesetNode)
	{
		Tooltip = MovesetNode->GetClass()->GetToolTipText();
	}
	if(Tooltip.IsEmpty())
	{
		Tooltip = GetNodeTitle(ENodeTitleType::ListView);
	}
	return Tooltip;
}

void UMovesetGraphNode_Move::CreateInputPins()
{
	UEdGraphPin* NewPin = CreatePin(EGPD_Input, UMovesetGraphSchema::PC_Exec, UMovesetGraphSchema::PSC_ExecMove, nullptr, false, false, TEXT("Exec In"));
	NewPin->PinFriendlyName = FText::FromString(TEXT(" "));

	for(int i = 0; i < MovesetNode->ChildNodes.Num() - 1; ++i)
	{
		UEdGraphPin* ChildPin = CreatePin(EGPD_Input, UMovesetGraphSchema::PC_FrameData, TEXT(""), nullptr, false, false, CreateUniquePinName(TEXT("Input")));
		ChildPin->PinFriendlyName = FText::FromString(TEXT(" "));
	}
}

void UMovesetGraphNode_Move::CreateOutputPins()
{
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, TEXT(""), nullptr, false, false, CreateUniquePinName(TEXT("Update")));
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, TEXT(""), nullptr, false, false, CreateUniquePinName(TEXT("Finish")));

	for(UMovesetNode_Base* ChildNode : MovesetNode->ChildNodes)
	{
		UMovesetNode_FrameData* FrameDataNode = Cast<UMovesetNode_FrameData>(ChildNode);
		if(FrameDataNode != nullptr)
		{
			ULinkFrames* LinkFrameData = Cast<ULinkFrames>(FrameDataNode->FrameData);
			if(LinkFrameData != nullptr)
			{
				CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, TEXT(""), nullptr, false, false, CreateUniquePinName(FString::Printf(TEXT("Link %d"), LinkFrameData->LinkIndex)));
			}
		}
	}
}
