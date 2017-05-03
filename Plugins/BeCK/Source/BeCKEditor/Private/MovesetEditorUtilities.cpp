// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#include "BeCKEditor.h"

#include "MovesetEditorUtilities.h"
#include "Toolkits/ToolkitManager.h"
#include "GraphEditor.h"
#include "Moveset.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "IMovesetEditor.h"
#include "Move.h"
#include "MovesetNode_Move.h"
#include "MovesetGraph.h"
#include "MovesetGraphSchema.h"
#include "MovesetGraphNode_Move.h"

bool FMovesetEditorUtilities::CanPasteNodes(const class UEdGraph* Graph)
{
	bool bCanPaste = false;
	TSharedPtr<class IMovesetEditor> MovesetEditor = GetIMovesetEditorForObject(Graph);
	if(MovesetEditor.IsValid())
	{
		bCanPaste = MovesetEditor->CanPasteNodes();
	}
	return bCanPaste;
}

void FMovesetEditorUtilities::PasteNodesHere(class UEdGraph* Graph, const FVector2D& Location)
{
	TSharedPtr<class IMovesetEditor> MovesetEditor = GetIMovesetEditorForObject(Graph);
	if(MovesetEditor.IsValid())
	{
		MovesetEditor->PasteNodesHere(Location);
	}
}

void FMovesetEditorUtilities::CreateFlipbookContainers(TArray<UPaperFlipbook*>& SelectedFlipbooks, UMoveset* Moveset, TArray<UMovesetNode_Base*>& OutMoves, FVector2D Location)
{
	const int32 NodeSpacing = 70;

	Location.Y -= static_cast<float>((SelectedFlipbooks.Num() - 1) * NodeSpacing) / 2.f;

	for(int32 WaveIndex = 0; WaveIndex < SelectedFlipbooks.Num(); WaveIndex++)
	{
		UPaperFlipbook* Flipbook = SelectedFlipbooks[WaveIndex];
		if(Flipbook)
		{
			UMovesetNode_Move* MoveNode = Moveset->ConstructMoveNode<UMovesetNode_Move>();
			MoveNode->Move->Flipbook = Flipbook;

			MoveNode->GraphNode->NodePosX = Location.X - CastChecked<UMovesetGraphNode_Move>(MoveNode->GetGraphNode())->EstimateNodeWidth();
			MoveNode->GraphNode->NodePosY = Location.Y + (NodeSpacing * WaveIndex);

			OutMoves.Add(MoveNode);
		}
	}
}

bool FMovesetEditorUtilities::GetBoundsForSelectedNodes(const UEdGraph* Graph, class FSlateRect& Rect, float Padding)
{
	TSharedPtr<class IMovesetEditor> MovesetEditor = GetIMovesetEditorForObject(Graph);
	if(MovesetEditor.IsValid())
	{
		return MovesetEditor->GetBoundsForSelectedNodes(Rect, Padding);
	}
	return false;
}

int32 FMovesetEditorUtilities::GetNumberOfSelectedNodes(const UEdGraph* Graph)
{
	TSharedPtr<class IMovesetEditor> MovesetEditor = GetIMovesetEditorForObject(Graph);
	if(MovesetEditor.IsValid())
	{
		return MovesetEditor->GetNumberOfSelectedNodes();
	}
	return 0;
}

TSet<UObject*> FMovesetEditorUtilities::GetSelectedNodes(const UEdGraph* Graph)
{
	TSharedPtr<class IMovesetEditor> MovesetEditor = GetIMovesetEditorForObject(Graph);
	if(MovesetEditor.IsValid())
	{
		return MovesetEditor->GetSelectedNodes();
	}
	return TSet<UObject*>();
}

TSharedPtr<class IMovesetEditor> FMovesetEditorUtilities::GetIMovesetEditorForObject(const UObject* ObjectToFocusOn)
{
	check(ObjectToFocusOn);

	// Find the associated Moveset
	UMoveset* Moveset = Cast<const UMovesetGraph>(ObjectToFocusOn)->GetMoveset();

	TSharedPtr<IMovesetEditor> MovesetEditor;
	if(Moveset != nullptr)
	{
		TSharedPtr< IToolkit > FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(Moveset);
		if(FoundAssetEditor.IsValid())
		{
			MovesetEditor = StaticCastSharedPtr<IMovesetEditor>(FoundAssetEditor);
		}
	}
	return MovesetEditor;
}

void FMovesetEditorUtilities::RefreshGraphNodes(const UEdGraph* Graph)
{
	TArray<UMovesetGraphNode_Base*> AllNodes;
	Graph->GetNodesOfClass(AllNodes);

	for(UMovesetGraphNode_Base* Node : AllNodes)
	{
		const UEdGraphSchema* Schema = Node->GetGraph()->GetSchema();
		Schema->ReconstructNode(*Node, true);
	}
}