// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpEditor.h"
#include "MovesetEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EdGraph/EdGraphNode.h"
#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"
#include "MovesetGraph/MovesetGraph.h"
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraph/MovesetGraphNode_Wildcard.h"
#include "MovesetGraph/MovesetGraphNode_Move.h"
#include "MovesetGraph/MovesetGraphSchema.h"
#include "Moveset.h"
#include "ScopedTransaction.h"
#include "GraphEditor.h"
#include "GraphEditorActions.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "SNodePanel.h"
#include "Editor.h"
#include "MovesetGraphEditorCommands.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Commands/GenericCommands.h"
#include "MovesetEditorUtilities.h"

#include "MovesetNode_Base.h"

#define LOCTEXT_NAMESPACE "MovesetEditor"

const FName FMovesetEditor::GraphCanvasTabId(TEXT("MovesetEditor_GraphCanvas"));
const FName FMovesetEditor::PropertiesTabId(TEXT("Moveset_Properties"));

FMovesetEditor::FMovesetEditor()
	: Moveset(nullptr)
{
}

void FMovesetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_MovesetEditor", "BeatEmUp Moveset Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(GraphCanvasTabId, FOnSpawnTab::CreateSP(this, &FMovesetEditor::SpawnTab_GraphCanvas))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FMovesetEditor::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FMovesetEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(GraphCanvasTabId);
	InTabManager->UnregisterTabSpawner(PropertiesTabId);
}

FMovesetEditor::~FMovesetEditor()
{
	GEditor->UnregisterForUndo(this);
}

void FMovesetEditor::InitMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{
	Moveset = CastChecked<UMoveset>(ObjectToEdit);

	// Support undo/redo
	Moveset->SetFlags(RF_Transactional);

	GEditor->RegisterForUndo(this);

	FGraphEditorCommands::Register();
	FMovesetGraphEditorCommands::Register();

	BindGraphCommands();

	CreateInternalWidgets();

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_MovesetEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.225f)
					->AddTab(PropertiesTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(GraphCanvasTabId, ETabState::OpenedTab)->SetHideTabWell(true)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, TEXT("MovesetEditorApp"), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectToEdit, false);

	ExtendToolbar();
	RegenerateMenusAndToolbars();

	// @todo toolkit world centric editing
	/*if(IsWorldCentricAssetEditor())
	{
	SpawnToolkitTab(GetToolbarTabId(), FString(), EToolkitTabSpot::ToolBar);
	SpawnToolkitTab(GraphCanvasTabId, FString(), EToolkitTabSpot::Viewport);
	SpawnToolkitTab(PropertiesTabId, FString(), EToolkitTabSpot::Details);
	}*/
}

UMoveset* FMovesetEditor::GetMoveset() const
{
	return Moveset;
}

void FMovesetEditor::SetSelection(TArray<UObject*> SelectedObjects)
{
	if(MovesetProperties.IsValid())
	{
		MovesetProperties->SetObjects(SelectedObjects);
	}
}

bool FMovesetEditor::GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding)
{
	return MovesetGraphEditor->GetBoundsForSelectedNodes(Rect, Padding);
}

int32 FMovesetEditor::GetNumberOfSelectedNodes() const
{
	return MovesetGraphEditor->GetSelectedNodes().Num();
}

FName FMovesetEditor::GetToolkitFName() const
{
	return FName("BeatEmUp MovesetEditor");
}

FText FMovesetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "BeatEmUp Moveset Editor");
}

FString FMovesetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "BeatEmUp Moveset ").ToString();
}

FLinearColor FMovesetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

TSharedRef<SDockTab> FMovesetEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == GraphCanvasTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("MovesetGraphCanvasTitle", "Viewport"));

	if(MovesetGraphEditor.IsValid())
	{
		SpawnedTab->SetContent(MovesetGraphEditor.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FMovesetEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("MovesetDetailsTitle", "Details"))
		[
			MovesetProperties.ToSharedRef()
		];
}

void FMovesetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Moveset);
}

void FMovesetEditor::PostUndo(bool bSuccess)
{
	if(MovesetGraphEditor.IsValid())
	{
		MovesetGraphEditor->ClearSelectionSet();
		MovesetGraphEditor->NotifyGraphChanged();
	}

}

void FMovesetEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, class UProperty* PropertyThatChanged)
{
	if(MovesetGraphEditor.IsValid() && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

		if(PropertyName == GET_MEMBER_NAME_CHECKED(UMoveset, InputSchema))
		{
			FMovesetEditorUtilities::RefreshGraphNodes(MovesetGraphEditor->GetCurrentGraph());
		}

		MovesetGraphEditor->NotifyGraphChanged();
	}
}

void FMovesetEditor::CreateInternalWidgets()
{
	MovesetGraphEditor = CreateGraphEditorWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	MovesetProperties = PropertyModule.CreateDetailView(Args);
	MovesetProperties->SetObject(Moveset);
}

void FMovesetEditor::ExtendToolbar()
{
	/*
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Toolbar");
			{
				ToolbarBuilder.AddToolBarButton(FMovesetGraphEditorCommands::Get().PlayCue);

				ToolbarBuilder.AddToolBarButton(FMovesetGraphEditorCommands::Get().PlayNode);

				ToolbarBuilder.AddToolBarButton(FMovesetGraphEditorCommands::Get().StopCueNode);
			}
			ToolbarBuilder.EndSection();
		}
	};

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar)
	);

	AddToolbarExtender(ToolbarExtender);

	IAudioEditorModule* AudioEditorModule = &FModuleManager::LoadModuleChecked<IAudioEditorModule>("AudioEditor");
	AddToolbarExtender(AudioEditorModule->GetMovesetToolBarExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));
	*/
}

void FMovesetEditor::BindGraphCommands()
{
	ToolkitCommands->MapAction(
		FGenericCommands::Get().Undo,
		FExecuteAction::CreateSP(this, &FMovesetEditor::UndoGraphAction));

	ToolkitCommands->MapAction(
		FGenericCommands::Get().Redo,
		FExecuteAction::CreateSP(this, &FMovesetEditor::RedoGraphAction));
}

void FMovesetEditor::AddInput()
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	// Iterator used but should only contain one node
	for(FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UMovesetGraphNode_Base* SelectedNode = Cast<UMovesetGraphNode_Base>(*NodeIt);

		if(SelectedNode)
		{
			SelectedNode->AddInputPin();
			break;
		}
	}
}

bool FMovesetEditor::CanAddInput() const
{
	return GetSelectedNodes().Num() == 1;
}

void FMovesetEditor::DeleteInput()
{
	UEdGraphPin* SelectedPin = MovesetGraphEditor->GetGraphPinForMenu();

	UMovesetGraphNode_Base* SelectedNode = Cast<UMovesetGraphNode_Base>(SelectedPin->GetOwningNode());

	if(SelectedNode && SelectedNode == SelectedPin->GetOwningNode())
	{
		SelectedNode->RemoveInputPin(SelectedPin);
	}
}

bool FMovesetEditor::CanDeleteInput() const
{
	return true;
}

void FMovesetEditor::OnCreateComment()
{
	FMovesetGraphSchemaAction_NewComment CommentAction;
	CommentAction.PerformAction(Moveset->MovesetGraph, nullptr, MovesetGraphEditor->GetPasteLocation());
}

TSharedRef<SGraphEditor> FMovesetEditor::CreateGraphEditorWidget()
{
	if(!GraphEditorCommands.IsValid())
	{
		GraphEditorCommands = MakeShareable(new FUICommandList);


		GraphEditorCommands->MapAction(FMovesetGraphEditorCommands::Get().AddInput,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::AddInput),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanAddInput));

		GraphEditorCommands->MapAction(FMovesetGraphEditorCommands::Get().DeleteInput,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::DeleteInput),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanDeleteInput));

		// Graph Editor Commands
		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().CreateComment,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::OnCreateComment)
		);

		// Editing commands
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::SelectAllNodes),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::DeleteSelectedNodes),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanDeleteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::CopySelectedNodes),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanCopyNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::CutSelectedNodes),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanCutNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::PasteNodes),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanPasteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::DuplicateNodes),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanDuplicateNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
									   FExecuteAction::CreateSP(this, &FMovesetEditor::RenameNodes),
									   FCanExecuteAction::CreateSP(this, &FMovesetEditor::CanRenameNodes)
		);
	}

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_Moveset", "MOVESET");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FMovesetEditor::OnSelectedNodesChanged);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FMovesetEditor::OnNodeTitleCommitted);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(Moveset->GetGraph())
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

FGraphPanelSelectionSet FMovesetEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if(MovesetGraphEditor.IsValid())
	{
		CurrentSelection = MovesetGraphEditor->GetSelectedNodes();
	}
	return CurrentSelection;
}

void FMovesetEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	if(NewSelection.Num())
	{
		for(TSet<class UObject*>::TConstIterator SetIt(NewSelection); SetIt; ++SetIt)
		{
			if(UMovesetGraphNode_Base* GraphNode = Cast<UMovesetGraphNode_Base>(*SetIt))
			{
				Selection.Add(GraphNode->MovesetNode);
			}
			else
			{
				Selection.Add(*SetIt);
			}
		}
		//Selection = NewSelection.Array();
	}
	else
	{
		Selection.Add(GetMoveset());
	}

	SetSelection(Selection);
}

void FMovesetEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{
	if(NodeBeingChanged)
	{
		const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

void FMovesetEditor::SelectAllNodes()
{
	MovesetGraphEditor->SelectAllNodes();
}

bool FMovesetEditor::CanSelectAllNodes() const
{
	return true;
}

void FMovesetEditor::DeleteSelectedNodes()
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "MovesetEditorDeleteSelectedNode", "Delete Selected BeatEmUp Moveset Node"));

	MovesetGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	MovesetGraphEditor->ClearSelectionSet();

	for(FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = CastChecked<UEdGraphNode>(*NodeIt);

		if(Node->CanUserDeleteNode())
		{
			if(UMovesetGraphNode_Base* MovesetGraphNode = Cast<UMovesetGraphNode_Base>(Node))
			{
				UMovesetNode_Base* DelNode = MovesetGraphNode->MovesetNode;

				FBlueprintEditorUtils::RemoveNode(nullptr, MovesetGraphNode, true);

				// Make sure Moveset is updated to match graph
				Moveset->CompileMoveNodesFromGraphNodes();

				// Remove this node from the Moveset's list of all SoundNodes
				Moveset->AllNodes.Remove(DelNode);
				Moveset->MarkPackageDirty();
			}
			else
			{
				FBlueprintEditorUtils::RemoveNode(nullptr, Node, true);
			}
		}
	}
}

bool FMovesetEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	if(SelectedNodes.Num() == 1)
	{
		for(FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			if(Cast<UMovesetGraphNode_Wildcard>(*NodeIt))
			{
				// Return false if only root node is selected, as it can't be deleted
				return false;
			}
		}
	}

	return SelectedNodes.Num() > 0;
}

void FMovesetEditor::DeleteSelectedDuplicatableNodes()
{
	// Cache off the old selection
	const FGraphPanelSelectionSet OldSelectedNodes = GetSelectedNodes();

	// Clear the selection and only select the nodes that can be duplicated
	FGraphPanelSelectionSet RemainingNodes;
	MovesetGraphEditor->ClearSelectionSet();

	for(FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if((Node != nullptr) && Node->CanDuplicateNode())
		{
			MovesetGraphEditor->SetNodeSelection(Node, true);
		}
		else
		{
			RemainingNodes.Add(Node);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	// Reselect whatever's left from the original selection after the deletion
	MovesetGraphEditor->ClearSelectionSet();

	for(FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter)
	{
		if(UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			MovesetGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FMovesetEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	// Cut should only delete nodes that can be duplicated
	DeleteSelectedDuplicatableNodes();
}

bool FMovesetEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FMovesetEditor::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for(FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if(UMovesetGraphNode_Base* Node = Cast<UMovesetGraphNode_Base>(*SelectedIter))
		{
			Node->PrepareForCopying();
		}
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
	FPlatformMisc::ClipboardCopy(*ExportedText);

	// Make sure Moveset remains the owner of the copied nodes
	for(FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if(UMovesetGraphNode_Base* Node = Cast<UMovesetGraphNode_Base>(*SelectedIter))
		{
			Node->PostCopyNode();
		}
	}
}

bool FMovesetEditor::CanCopyNodes() const
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for(FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if((Node != nullptr) && Node->CanDuplicateNode())
		{
			return true;
		}
	}
	return false;
}

void FMovesetEditor::PasteNodes()
{
	PasteNodesHere(MovesetGraphEditor->GetPasteLocation());
}

void FMovesetEditor::PasteNodesHere(const FVector2D& Location)
{
	// Undo/Redo support
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "MovesetEditorPaste", "Paste BeatEmUp Moveset Node"));
	Moveset->GetGraph()->Modify();
	Moveset->Modify();

	// Clear the selection set (newly pasted stuff will be selected)
	MovesetGraphEditor->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(Moveset->GetGraph(), TextToImport, /*out*/ PastedNodes);

	//Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f, 0.0f);

	for(TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;
	}

	if(PastedNodes.Num() > 0)
	{
		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	for(TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;

		if(UMovesetGraphNode_Base* MovesetGraphNode = Cast<UMovesetGraphNode_Base>(Node))
		{
			Moveset->AllNodes.Add(MovesetGraphNode->MovesetNode);
		}

		// Select the newly pasted stuff
		MovesetGraphEditor->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	// Force new pasted SoundNodes to have same connections as graph nodes
	//Moveset->CompileSoundNodesFromGraphNodes();

	// Update UI
	MovesetGraphEditor->NotifyGraphChanged();

	Moveset->PostEditChange();
	Moveset->MarkPackageDirty();
}

bool FMovesetEditor::CanPasteNodes() const
{
	FString ClipboardContent;
	FPlatformMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(Moveset->MovesetGraph, ClipboardContent);
}

void FMovesetEditor::DuplicateNodes()
{
	// Copy and paste current selection
	CopySelectedNodes();
	PasteNodes();
}

bool FMovesetEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FMovesetEditor::RenameNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for(FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
		if(SelectedNode != nullptr && SelectedNode->bCanRenameNode)
		{
			MovesetGraphEditor->JumpToNode(SelectedNode, true);
			break;
		}
	}
}

bool FMovesetEditor::CanRenameNodes() const
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for(FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);

		if((Node != nullptr) && Node->bCanRenameNode)
		{
			return true;
		}
	}
	return false;
}

void FMovesetEditor::UndoGraphAction()
{
	GEditor->UndoTransaction();
}

void FMovesetEditor::RedoGraphAction()
{
	// Clear selection, to avoid holding refs to nodes that go away
	MovesetGraphEditor->ClearSelectionSet();

	GEditor->RedoTransaction();
}

#undef LOCTEXT_NAMESPACE
