// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
MovesetGraphSchema.cpp
=============================================================================*/

#include "BeatEmUpEditor.h"

#include "MovesetGraph/MovesetGraphSchema.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "Layout/SlateRect.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EdGraphNode_Comment.h"
#include "EdGraph/EdGraph.h"
#include "MovesetGraph/MovesetGraph.h"
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraph/MovesetGraphNode_Root.h"
#include "MovesetGraph/MovesetGraphNode_Wildcard.h"
#include "MovesetGraph/MovesetGraphNode_Move.h"
#include "Editor.h"
#include "MovesetNode_Base.h"
#include "GraphEditorActions.h"
#include "MovesetEditorUtilities.h"
#include "ScopedTransaction.h"
#include "GraphEditor.h"
#include "Engine/Selection.h"
#include "Moveset.h"

#define LOCTEXT_NAMESPACE "MovesetSchema"

TArray<UClass*> UMovesetGraphSchema::MoveNodeClasses;
bool UMovesetGraphSchema::bMoveNodeClassesInitialized = false;

const FString UMovesetGraphSchema::PC_Exec(TEXT("exec"));
const FString UMovesetGraphSchema::PC_Byte(TEXT("byte"));
const FString UMovesetGraphSchema::PC_FrameData(TEXT("frame-data"));
const FString UMovesetGraphSchema::PC_RedirectTarget(TEXT("redirect-target"));

const FString UMovesetGraphSchema::PSC_ExecRoot(TEXT("exec_root"));
const FString UMovesetGraphSchema::PSC_ExecMove(TEXT("exec_move"));

const FString UMovesetGraphSchema::PSC_ByteInput(TEXT("byte_input"));

/////////////////////////////////////////////////////
// FMovesetGraphSchemaAction_NewNode

UEdGraphNode* FMovesetGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	check(MoveNodeClass);

	UMoveset* Moveset = CastChecked<UMovesetGraph>(ParentGraph)->GetMoveset();
	const FScopedTransaction Transaction(LOCTEXT("MovesetEditorNewMoveNode", "Sound Cue Editor: New Sound Node"));
	ParentGraph->Modify();
	Moveset->Modify();

	UMovesetNode_Base* NewNode = Moveset->ConstructMoveNode<UMovesetNode_Base>(MoveNodeClass, bSelectNewNode);

	// Attempt to connect inputs to selected nodes, unless we're already dragging from a single output
	if(FromPin == nullptr || FromPin->Direction == EGPD_Input)
	{
		ConnectToSelectedNodes(NewNode, ParentGraph);
	}

	NewNode->GraphNode->NodePosX = Location.X;
	NewNode->GraphNode->NodePosY = Location.Y;

	NewNode->GraphNode->AutowireNewNode(FromPin);

	Moveset->PostEditChange();
	Moveset->MarkPackageDirty();

	return NewNode->GraphNode;
}

void FMovesetGraphSchemaAction_NewNode::ConnectToSelectedNodes(UMovesetNode_Base* NewNode, class UEdGraph* ParentGraph) const
{
	// only connect if node can have many children
	if(NewNode->GetMaxChildNodes() > 1)
	{
		const FGraphPanelSelectionSet SelectedNodes = FMovesetEditorUtilities::GetSelectedNodes(ParentGraph);

		TArray<UMovesetNode_Base*> SortedNodes;
		for(FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UMovesetGraphNode_Base* SelectedNode = Cast<UMovesetGraphNode_Base>(*NodeIt);

			if(SelectedNode)
			{
				// Sort the nodes by y position
				bool bInserted = false;
				for(int32 Index = 0; Index < SortedNodes.Num(); ++Index)
				{
					if(SortedNodes[Index]->GraphNode->NodePosY > SelectedNode->NodePosY)
					{
						SortedNodes.Insert(SelectedNode->MovesetNode, Index);
						bInserted = true;
						break;
					}
				}
				if(!bInserted)
				{
					SortedNodes.Add(SelectedNode->MovesetNode);
				}
			}
		}
		if(SortedNodes.Num() > 1)
		{
			CastChecked<UMovesetGraphSchema>(NewNode->GraphNode->GetSchema())->TryConnectNodes(SortedNodes, NewNode);
		}
	}
}

/////////////////////////////////////////////////////
// FMovesetGraphSchemaAction_NewFromSelected

UEdGraphNode* FMovesetGraphSchemaAction_NewFromSelected::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UMoveset* Moveset = CastChecked<UMovesetGraph>(ParentGraph)->GetMoveset();
	const FScopedTransaction Transaction(LOCTEXT("MovesetEditorNewFromSelection", "Moveset Editor: New From Selection"));
	ParentGraph->Modify();
	Moveset->Modify();

	UEdGraphNode* CreatedNode = nullptr;

	FVector2D MoveStartLocation = Location;

	if(MoveNodeClass)
	{
		// If we will create another node, move wave nodes out of the way.
		MoveStartLocation.X -= 200;
	}

	TArray<UPaperFlipbook*> SelectedFlipbooks;
	TArray<UMovesetNode_Base*> CreatedMoves;

	GEditor->GetSelectedObjects()->GetSelectedObjects<UPaperFlipbook>(SelectedFlipbooks);

	FMovesetEditorUtilities::CreateFlipbookContainers(SelectedFlipbooks, Moveset, CreatedMoves, MoveStartLocation);

	if(MoveNodeClass)
	{
		UMovesetNode_Base* NewNode = Moveset->ConstructMoveNode<UMovesetNode_Base>(MoveNodeClass, bSelectNewNode);
		UEdGraphNode* NewGraphNode = NewNode->GraphNode;
		const UMovesetGraphSchema* NewSchema = CastChecked<UMovesetGraphSchema>(NewGraphNode->GetSchema());

		NewSchema->TryConnectNodes(CreatedMoves, NewNode);

		NewGraphNode->NodePosX = Location.X;
		NewGraphNode->NodePosY = Location.Y;

		CreatedNode = NewNode->GraphNode;
	}
	else
	{
		if(CreatedMoves.Num() > 0)
		{
			CreatedNode = CreatedMoves[0]->GraphNode;
		}
	}

	if(CreatedNode)
	{
		CreatedNode->AutowireNewNode(FromPin);
	}

	Moveset->PostEditChange();
	Moveset->MarkPackageDirty();

	return CreatedNode;
}

/////////////////////////////////////////////////////
// FMovesetGraphSchemaAction_NewComment

UEdGraphNode* FMovesetGraphSchemaAction_NewComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	// Add menu item for creating comment boxes
	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;

	FSlateRect Bounds;
	if(FMovesetEditorUtilities::GetBoundsForSelectedNodes(ParentGraph, Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation);
}

/////////////////////////////////////////////////////
// FMovesetGraphSchemaAction_Paste

UEdGraphNode* FMovesetGraphSchemaAction_Paste::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	FMovesetEditorUtilities::PasteNodesHere(ParentGraph, Location);
	return nullptr;
}

/////////////////////////////////////////////////////
// UMovesetGraphSchema

UMovesetGraphSchema::UMovesetGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMovesetGraphSchema::TryConnectNodes(const TArray<UMovesetNode_Base*>& OutputNodes, UMovesetNode_Base* InputNode) const
{
	for(int32 Index = 0; Index < OutputNodes.Num(); Index++)
	{
		if(Index < InputNode->GetMaxChildNodes())
		{
			UMovesetGraphNode_Base* GraphNode = CastChecked<UMovesetGraphNode_Base>(InputNode->GetGraphNode());
			TryCreateConnection(GraphNode->GetInputPin(Index), CastChecked<UMovesetGraphNode_Base>(OutputNodes[Index]->GetGraphNode())->GetOutputPin(Index));
		}
	}
}

void UMovesetGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	GetAllMoveNodeActions(ContextMenuBuilder, true);

	GetCommentAction(ContextMenuBuilder, ContextMenuBuilder.CurrentGraph);

	if(!ContextMenuBuilder.FromPin && FMovesetEditorUtilities::CanPasteNodes(ContextMenuBuilder.CurrentGraph))
	{
		TSharedPtr<FMovesetGraphSchemaAction_Paste> NewAction(new FMovesetGraphSchemaAction_Paste(FText::GetEmpty(), LOCTEXT("PasteHereAction", "Paste here"), TEXT(""), 0));
		ContextMenuBuilder.AddAction(NewAction);
	}
}

void UMovesetGraphSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	if(InGraphPin)
	{
		MenuBuilder->BeginSection("MovesetGraphSchemaPinActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
		{
			// Only display the 'Break Link' option if there is a link to break!
			if(InGraphPin->LinkedTo.Num() > 0)
			{
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
			}
		}
		MenuBuilder->EndSection();
	}
	else if(InGraphNode)
	{
		const UMovesetGraphNode_Base* SoundGraphNode = Cast<const UMovesetGraphNode_Base>(InGraphNode);

		MenuBuilder->BeginSection("MovesetGraphSchemaNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
		{
			MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
			MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ReconstructNodes);
		}
		MenuBuilder->EndSection();
	}

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

void UMovesetGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Create the root node
	FGraphNodeCreator<UMovesetGraphNode_Root> RootNodeCreator(Graph);
	UMovesetGraphNode_Root* ResultRootNode = RootNodeCreator.CreateNode();
	RootNodeCreator.Finalize();
	SetNodeMetaData(ResultRootNode, FNodeMetadata::DefaultGraphNode);

	// Create the wildcard node
	FGraphNodeCreator<UMovesetGraphNode_Wildcard> WildcardNodeCreator(Graph);
	UMovesetGraphNode_Wildcard* ResultWildcardNode = WildcardNodeCreator.CreateNode();
	ResultWildcardNode->NodePosY = 128;
	WildcardNodeCreator.Finalize();
	SetNodeMetaData(ResultWildcardNode, FNodeMetadata::DefaultGraphNode);
}

const FPinConnectionResponse UMovesetGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if(PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionSameNode", "Both are on the same node"));
	}

	// Make sure the pins are of the same type
	if(PinA->PinType.PinCategory != PinB->PinType.PinCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("TypesIncompatible", "Types are not compatible"));
	}

	// Compare the directions
	const UEdGraphPin* InputPin = nullptr;
	const UEdGraphPin* OutputPin = nullptr;

	if(!CategorizePinsByDirection(PinA, PinB, InputPin, OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionIncompatible", "Directions are not compatible"));
	}

	// Make sure the root node isn't being connected to non-move
	if(OutputPin->PinType.PinSubCategory == UMovesetGraphSchema::PSC_ExecRoot && PinB->PinType.PinSubCategory != UMovesetGraphSchema::PSC_ExecMove)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("RootNodeConnectedToNonMove", "Root node must be connected to a move node"));
	}

	// Make sure exec outputs don't have multiple connections
	if(OutputPin->PinType.PinCategory == UMovesetGraphSchema::PC_Exec)
	{
		if(OutputPin == PinA)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, FText::GetEmpty());
		}
		else if(OutputPin == PinB)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, FText::GetEmpty());
		}
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, FText::GetEmpty());
}

bool UMovesetGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);

	if(bModified)
	{
		CastChecked<UMovesetGraph>(PinA->GetOwningNode()->GetGraph())->GetMoveset()->CompileMoveNodesFromGraphNodes();
	}

	return bModified;
}

bool UMovesetGraphSchema::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return false;
}

FLinearColor UMovesetGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if(PinType.PinCategory == UMovesetGraphSchema::PC_Byte)
	{
		if(PinType.PinSubCategory == UMovesetGraphSchema::PSC_ByteInput)
		{
			return FLinearColor::Red * 0.8f;
		}
	}
	else if(PinType.PinCategory == UMovesetGraphSchema::PC_FrameData)
	{
		return FLinearColor(0.2f, 0.2f, 1.0f);
	}

	return FLinearColor::White;
}

void UMovesetGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	Super::BreakNodeLinks(TargetNode);

	CastChecked<UMovesetGraph>(TargetNode.GetGraph())->GetMoveset()->CompileMoveNodesFromGraphNodes();
}

void UMovesetGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);

	// if this would notify the node then we need to compile the Moveset
	if(bSendsNodeNotifcation)
	{
		CastChecked<UMovesetGraph>(TargetPin.GetOwningNode()->GetGraph())->GetMoveset()->CompileMoveNodesFromGraphNodes();
	}
}

void UMovesetGraphSchema::DroppedAssetsOnGraph(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const
{
	//////////////////////////////////////////////////////////////////////////
	// Handle dropped UPaperFlipbooks
	TArray<UPaperFlipbook*> Flipbooks;
	for(int32 AssetIdx = 0; AssetIdx < Assets.Num(); ++AssetIdx)
	{
		UPaperFlipbook* Flipbook = Cast<UPaperFlipbook>(Assets[AssetIdx].GetAsset());
		if(Flipbook)
		{
			Flipbooks.Add(Flipbook);
		}
	}

	if(Flipbooks.Num() > 0)
	{
		const FScopedTransaction Transaction(LOCTEXT("MovesetEditorDropFlipbook", "Sound Cue Editor: Drag and Drop Paper Flipbook"));

		UMovesetGraph* MovesetGraph = CastChecked<UMovesetGraph>(Graph);
		UMoveset* Moveset = MovesetGraph->GetMoveset();

		MovesetGraph->Modify();

		TArray<UMovesetNode_Base*> CreatedMoves;
		FMovesetEditorUtilities::CreateFlipbookContainers(Flipbooks, Moveset, CreatedMoves, GraphPosition);
	}
}

void UMovesetGraphSchema::DroppedAssetsOnNode(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraphNode* Node) const
{
	if(!Node->IsA<UMovesetGraphNode_Base>())
	{
		return;
	}

	UMovesetGraphNode_Base* MovesetGraphNode = CastChecked<UMovesetGraphNode_Base>(Node);
	UMovesetGraph* MovesetGraph = CastChecked<UMovesetGraph>(Node->GetGraph());
	UMoveset* Moveset = MovesetGraph->GetMoveset();

	TArray<UPaperFlipbook*> Flipbooks;
	for(int32 AssetIdx = 0; AssetIdx < Assets.Num(); ++AssetIdx)
	{
		UPaperFlipbook* Flipbook = Cast<UPaperFlipbook>(Assets[AssetIdx].GetAsset());
		if(Flipbook)
		{
			Flipbooks.Add(Flipbook);
		}
	}

	MovesetGraph->NotifyGraphChanged();
}

void UMovesetGraphSchema::GetAllMoveNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, bool bShowSelectedActions) const
{
	InitMoveNodeClasses();

	FText SelectedItemText;
	bool IsFlipbookSelected = false;
	bool IsDialogueWaveSelected = false;

	if(bShowSelectedActions)
	{
		FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

		// Get display text for any items that may be selected
		if(ActionMenuBuilder.FromPin == nullptr)
		{
			TArray<UPaperFlipbook*> SelectedFlipbooks;
			GEditor->GetSelectedObjects()->GetSelectedObjects<UPaperFlipbook>(SelectedFlipbooks);

			int32 TotalWavs = SelectedFlipbooks.Num();

			if(TotalWavs > 1)
			{
				SelectedItemText = LOCTEXT("MultipleFlipbooksSelected", "Multiple Flipbooks");
			}
			else if(SelectedFlipbooks.Num() == 1)
			{
				SelectedItemText = FText::FromString(SelectedFlipbooks[0]->GetName());
				IsFlipbookSelected = true;
			}
		}
		else
		{
			UPaperFlipbook* SelectedFlipbook = GEditor->GetSelectedObjects()->GetTop<UPaperFlipbook>();
			if(SelectedFlipbook && ActionMenuBuilder.FromPin->Direction == EGPD_Input)
			{
				SelectedItemText = FText::FromString(SelectedFlipbook->GetName());
				IsFlipbookSelected = true;
			}
		}

		bShowSelectedActions = !SelectedItemText.IsEmpty();
	}

	for(UClass* MoveNodeClass : MoveNodeClasses)
	{
		UMovesetNode_Base* MoveNode = MoveNodeClass->GetDefaultObject<UMovesetNode_Base>();

		// when dragging from an output pin you can create anything but a wave player
		if(!ActionMenuBuilder.FromPin || ActionMenuBuilder.FromPin->Direction == EGPD_Input || MoveNode->GetMaxChildNodes() > 0)
		{
			const FText Name = FText::FromString(MoveNodeClass->GetDescription());

			{
				FFormatNamedArguments Arguments;
				Arguments.Add(TEXT("Name"), Name);
				const FText AddToolTip = FText::Format(LOCTEXT("NewMovesetNodeTooltip", "Adds {Name} node here"), Arguments);
				TSharedPtr<FMovesetGraphSchemaAction_NewNode> NewNodeAction(new FMovesetGraphSchemaAction_NewNode(LOCTEXT("MoveNodeAction", "Sound Node"), Name, AddToolTip.ToString(), 0));
				ActionMenuBuilder.AddAction(NewNodeAction);
				NewNodeAction->MoveNodeClass = MoveNodeClass;
			}

			/*
			if(bShowSelectedActions &&
				(MoveNode->GetMaxChildNodes() == UMoveNode::MAX_ALLOWED_CHILD_NODES ||
			   ((MoveNodeClass == UMoveNodeWavePlayer::StaticClass() && IsFlipbookSelected))))
			{
				FFormatNamedArguments Arguments;
				Arguments.Add(TEXT("Name"), Name);
				Arguments.Add(TEXT("SelectedItems"), SelectedItemText);
				const FText MenuDesc = FText::Format(LOCTEXT("NewMoveNodeRandom", "{Name}: {SelectedItems}"), Arguments);
				const FText ToolTip = FText::Format(LOCTEXT("NewMoveNodeRandomTooltip", "Adds a {Name} node for {SelectedItems} here"), Arguments);
				TSharedPtr<FMovesetGraphSchemaAction_NewFromSelected> NewNodeAction(new FMovesetGraphSchemaAction_NewFromSelected(LOCTEXT("FromSelected", "From Selected"),
																																	MenuDesc,
																																	ToolTip.ToString(), 0));
				ActionMenuBuilder.AddAction(NewNodeAction);
				NewNodeAction->MoveNodeClass = (MoveNodeClass == UMoveNodeWavePlayer::StaticClass() ? nullptr : MoveNodeClass);
			}
			*/
		}
	}
}

void UMovesetGraphSchema::GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph) const
{
	if(!ActionMenuBuilder.FromPin)
	{
		const bool bIsManyNodesSelected = CurrentGraph ? (FMovesetEditorUtilities::GetNumberOfSelectedNodes(CurrentGraph) > 0) : false;
		const FText MenuDescription = bIsManyNodesSelected ? LOCTEXT("CreateCommentAction", "Create Comment from Selection") : LOCTEXT("AddCommentAction", "Add Comment...");
		const FString ToolTip = LOCTEXT("CreateCommentToolTip", "Creates a comment.").ToString();

		TSharedPtr<FMovesetGraphSchemaAction_NewComment> NewAction(new FMovesetGraphSchemaAction_NewComment(FText::GetEmpty(), MenuDescription, ToolTip, 0));
		ActionMenuBuilder.AddAction(NewAction);
	}
}

void UMovesetGraphSchema::InitMoveNodeClasses()
{
	if(bMoveNodeClassesInitialized)
	{
		return;
	}

	// Construct list of non-abstract move node classes.
	for(TObjectIterator<UClass> It; It; ++It)
	{
		if(It->IsChildOf(UMovesetNode_Base::StaticClass())
		   && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			MoveNodeClasses.Add(*It);
		}
	}

	MoveNodeClasses.Sort();

	bMoveNodeClassesInitialized = true;
}

int32 UMovesetGraphSchema::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	return FMovesetEditorUtilities::GetNumberOfSelectedNodes(Graph);
}

TSharedPtr<FEdGraphSchemaAction> UMovesetGraphSchema::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FMovesetGraphSchemaAction_NewComment));
}

#undef LOCTEXT_NAMESPACE
