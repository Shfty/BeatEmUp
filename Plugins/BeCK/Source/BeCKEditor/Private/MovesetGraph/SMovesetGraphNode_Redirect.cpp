// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeCKEditor.h"

#include "SMovesetGraphNode_Redirect.h"
#include "Widgets/SBoxPanel.h"
#include "SComboBox.h"

void SMovesetGraphNode_Redirect::Construct(const FArguments& InArgs, UMovesetGraphNode_Redirect* InNode)
{
	this->GraphNode = Cast<UEdGraphNode>(InNode);
	this->MovesetNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

TSharedRef<SWidget> SMovesetGraphNode_Redirect::CreateCenterContentArea()
{
	return SNullWidget::NullWidget;
}
