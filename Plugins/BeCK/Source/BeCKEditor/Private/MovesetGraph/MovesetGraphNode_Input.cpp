// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
MovesetGraphNode_Move.cpp
=============================================================================*/

#include "BeCKEditor.h"

#include "MovesetGraph/MovesetGraphNode_Input.h"
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
#include "MovesetNode_Input.h"

void UMovesetGraphNode_Input::CreateInputPins()
{
	UEdGraphPin* ExecPin = CreatePin(EGPD_Input, UMovesetGraphSchema::PC_Exec, FString(), nullptr, /*bIsArray=*/ false, /*bIsReference=*/ false, TEXT("Exec In"));
	ExecPin->PinFriendlyName = FText::FromString(TEXT(" "));

	UMovesetGraph* MovesetGraph = Cast<UMovesetGraph>(GetGraph());
	if(MovesetGraph != nullptr)
	{
		UMoveset* Moveset = MovesetGraph->GetMoveset();
		if(Moveset != nullptr)
		{
			UEnum* InputSchema = Moveset->InputSchema.LoadSynchronous();
			CreatePin(EGPD_Input, UMovesetGraphSchema::PC_Byte, UMovesetGraphSchema::PSC_ByteInput, InputSchema, false, false, TEXT("Input"));
		}
	}
}

void UMovesetGraphNode_Input::CreateOutputPins()
{
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, FString(), nullptr, false, false, TEXT("Pressed"));
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, FString(), nullptr, false, false, TEXT("Released"));
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, FString(), nullptr, false, false, TEXT("True"));
	CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, FString(), nullptr, false, false, TEXT("False"));
}

FLinearColor UMovesetGraphNode_Input::GetNodeTitleColor() const
{
	return FLinearColor::Red;
}

FText UMovesetGraphNode_Input::GetTooltipText() const
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
