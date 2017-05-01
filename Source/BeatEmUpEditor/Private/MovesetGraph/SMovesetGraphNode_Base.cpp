// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpEditor.h"

#include "SMovesetGraphNode_Base.h"
#include "Widgets/SBoxPanel.h"
#include "SBox.h"
#include "SScaleBox.h"
#include "GraphEditorSettings.h"

void SMovesetGraphNode_Base::Construct(const FArguments& InArgs, UMovesetGraphNode_Base* InNode)
{
	this->GraphNode = Cast<UEdGraphNode>(InNode);
	this->MovesetNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

TSharedRef<SWidget> SMovesetGraphNode_Base::CreateNodeContentArea()
{
	return SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				// LEFT
				SAssignNew(LeftNodeBox, SVerticalBox)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				// CENTER
				CreateCenterContentArea()
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Top)
			[
				// RIGHT
				SAssignNew(RightNodeBox, SVerticalBox)
			]
		];
}


void SMovesetGraphNode_Base::CreateBelowPinControls(TSharedPtr<SVerticalBox> OutputBox)
{
	TSharedRef<SWidget> AddPinButton = AddPinButtonContent(
		NSLOCTEXT("MoveNode", "MoveNodeAddPinButton", "Add frame data"),
		NSLOCTEXT("MoveNode", "MoveNodeAddPinButton_Tooltip", "Adds a frame data input to the move node")
	);

	FMargin AddPinPadding = Settings->GetOutputPinPadding();
	AddPinPadding.Top += 3.0f;
	AddPinPadding.Bottom += 3.0f;

	OutputBox->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(AddPinPadding)
		[
			AddPinButton
		];
}

EVisibility SMovesetGraphNode_Base::IsAddPinButtonVisible() const
{
	EVisibility ButtonVisibility = SGraphNode::IsAddPinButtonVisible();
	if(ButtonVisibility == EVisibility::Visible)
	{
		if(!MovesetNode->CanAddInputPin())
		{
			ButtonVisibility = EVisibility::Collapsed;
		}
	}
	return ButtonVisibility;
}

FReply SMovesetGraphNode_Base::OnAddPin()
{
	MovesetNode->AddInputPin();

	return FReply::Handled();
}
