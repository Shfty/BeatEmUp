// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.


#include "BeatEmUpEditor.h"
#include "SMovesetGraphPin_RedirectTarget.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/SListView.h"
#include "ScopedTransaction.h"
#include "MovesetGraphNode_RedirectTarget.h"
#include "MovesetNode_RedirectTarget.h"
#include "MovesetGraph.h"

void SMovesetGraphPin_RedirectTarget::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget>	SMovesetGraphPin_RedirectTarget::GetDefaultValueWidget()
{
	//Get list of enum indexes
	TArray< TSharedPtr<int32> > ComboItems;
	GenerateComboBoxIndexes(ComboItems);

	//Create widget
	return SAssignNew(ComboBox, SPinComboBox)
		.ComboItemList(ComboItems)
		.VisibleText(this, &SMovesetGraphPin_RedirectTarget::OnGetText)
		.OnSelectionChanged(this, &SMovesetGraphPin_RedirectTarget::ComboBoxSelectionChanged)
		.Visibility(this, &SMovesetGraphPin_RedirectTarget::GetDefaultValueVisibility)
		.OnGetDisplayName(this, &SMovesetGraphPin_RedirectTarget::OnGetFriendlyName)
		.OnGetTooltip(this, &SMovesetGraphPin_RedirectTarget::OnGetTooltip);
}

FText SMovesetGraphPin_RedirectTarget::OnGetFriendlyName(int32 Index)
{
	UEdGraphNode* Node = GetPinObj()->GetOwningNode();
	if(Node != nullptr)
	{
		UEdGraph* Graph = Node->GetGraph();
		if(Graph != nullptr)
		{
			UMovesetGraph* MovesetGraph = Cast<UMovesetGraph>(Graph);
			if(MovesetGraph != nullptr)
			{
				TArray<UMovesetGraphNode_RedirectTarget*> RedirectTargetNodes;
				MovesetGraph->GetNodesOfClass<UMovesetGraphNode_RedirectTarget>(RedirectTargetNodes);

				return RedirectTargetNodes[Index]->GetNodeTitle(ENodeTitleType::ListView);
			}
		}
	}

	return FText::GetEmpty();
}

FText SMovesetGraphPin_RedirectTarget::OnGetTooltip(int32 Index)
{
	return FText::GetEmpty();
}

void SMovesetGraphPin_RedirectTarget::ComboBoxSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type /*SelectInfo*/)
{
	TArray<UMovesetGraphNode_RedirectTarget*> RedirectTargetNodes;
	GetRedirectTargetNodes(RedirectTargetNodes);

	FString SelectionString;

	if(NewSelection.IsValid())
	{
		check(*NewSelection < RedirectTargetNodes.Num());
		SelectionString = RedirectTargetNodes[*NewSelection]->GetNodeTitle(ENodeTitleType::ListView).ToString();
	}
	else
	{
		SelectionString = FName(NAME_None).ToString();
	}


	if(GraphPinObj->GetDefaultAsString() != SelectionString)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeRedirectTargetPinValue", "Change Redirect Target Pin Value"));
		GraphPinObj->Modify();

		//Set new selection
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, SelectionString);

		UMovesetGraphNode_Base* OwningNode = Cast<UMovesetGraphNode_Base>(GraphPinObj->GetOwningNode());

		if(OwningNode != nullptr)
		{
			UMovesetNode_RedirectTarget* RedirectTargetNode = Cast<UMovesetNode_RedirectTarget>(OwningNode->MovesetNode);

			if(RedirectTargetNode != nullptr)
			{
				// Set RedirectTargetNode reference to selected value
			}
		}
	}
}

FString SMovesetGraphPin_RedirectTarget::OnGetText() const
{
	FString SelectedString = GraphPinObj->GetDefaultAsString();

	TArray<UMovesetGraphNode_RedirectTarget*> RedirectTargetNodes;
	GetRedirectTargetNodes(RedirectTargetNodes);

	for(int32 i = 0; i < RedirectTargetNodes.Num(); ++i)
	{
		if(SelectedString == RedirectTargetNodes[i]->GetNodeTitle(ENodeTitleType::ListView).ToString())
		{
			FString DisplayName = RedirectTargetNodes[i]->GetNodeTitle(ENodeTitleType::ListView).ToString();
			if(DisplayName.Len() == 0)
			{
				return SelectedString;
			}
			else
			{
				return DisplayName;
			}
		}
	}

	return SelectedString;
}

void SMovesetGraphPin_RedirectTarget::GenerateComboBoxIndexes(TArray< TSharedPtr<int32> >& OutComboBoxIndexes)
{
	TArray<UMovesetGraphNode_RedirectTarget*> RedirectTargetNodes;
	GetRedirectTargetNodes(RedirectTargetNodes);

	for(int32 i = 0; i < RedirectTargetNodes.Num(); ++i)
	{
		TSharedPtr<int32> IdxPtr(new int32(i));
		OutComboBoxIndexes.Add(IdxPtr);
	}
}

void SMovesetGraphPin_RedirectTarget::GetRedirectTargetNodes(TArray<UMovesetGraphNode_RedirectTarget*>& OutRedirectTargetNodes) const
{
	UEdGraphNode* Node = GetPinObj()->GetOwningNode();
	if(Node != nullptr)
	{
		UEdGraph* Graph = Node->GetGraph();
		if(Graph != nullptr)
		{
			UMovesetGraph* MovesetGraph = Cast<UMovesetGraph>(Graph);
			if(MovesetGraph != nullptr)
			{
				MovesetGraph->GetNodesOfClass<UMovesetGraphNode_RedirectTarget>(OutRedirectTargetNodes);
			}
		}
	}
}
