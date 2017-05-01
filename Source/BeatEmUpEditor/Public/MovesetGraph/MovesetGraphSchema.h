// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AssetData.h"
#include "EdGraph/EdGraphSchema.h"
#include "MovesetGraphSchema.generated.h"

class UEdGraph;
class UMovesetNode_Base;

/** Action to add a node to the graph */
USTRUCT()
struct BEATEMUPEDITOR_API FMovesetGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Class of node we want to create */
	UPROPERTY()
	class UClass* MoveNodeClass;


	FMovesetGraphSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, MoveNodeClass(nullptr)
	{
	}

	FMovesetGraphSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FString InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, MoveNodeClass(nullptr)
	{
	}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface

private:
	/** Connects new node to output of selected nodes */
	void ConnectToSelectedNodes(UMovesetNode_Base* NewNodeclass, UEdGraph* ParentGraph) const;
};

/** Action to add nodes to the graph based on selected objects*/
USTRUCT()
struct BEATEMUPEDITOR_API FMovesetGraphSchemaAction_NewFromSelected : public FMovesetGraphSchemaAction_NewNode
{
	GENERATED_USTRUCT_BODY();

	FMovesetGraphSchemaAction_NewFromSelected()
		: FMovesetGraphSchemaAction_NewNode()
	{
	}

	FMovesetGraphSchemaAction_NewFromSelected(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
		: FMovesetGraphSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	{
	}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface
};

/** Action to create new comment */
USTRUCT()
struct BEATEMUPEDITOR_API FMovesetGraphSchemaAction_NewComment : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	FMovesetGraphSchemaAction_NewComment()
		: FEdGraphSchemaAction()
	{
	}

	FMovesetGraphSchemaAction_NewComment(FText InNodeCategory, FText InMenuDesc, FString InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface
};

/** Action to paste clipboard contents into the graph */
USTRUCT()
struct BEATEMUPEDITOR_API FMovesetGraphSchemaAction_Paste : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	FMovesetGraphSchemaAction_Paste()
		: FEdGraphSchemaAction()
	{
	}

	FMovesetGraphSchemaAction_Paste(FText InNodeCategory, FText InMenuDesc, FString InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{
	}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface
};

UCLASS(MinimalAPI)
class UMovesetGraphSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

	static const FString PC_Exec;
	static const FString PC_Byte;
	static const FString PC_FrameData;
	static const FString PC_RedirectTarget;

	static const FString PSC_ExecRoot;
	static const FString PSC_ExecMove;

	static const FString PSC_ByteInput;

	/** Attempts to connect the output of multiple nodes to the inputs of a single one */
	void TryConnectNodes(const TArray<UMovesetNode_Base*>& OutputNodes, UMovesetNode_Base* InputNode) const;

	//~ Begin EdGraphSchema Interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;
	virtual void DroppedAssetsOnGraph(const TArray<class FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const override;
	virtual void DroppedAssetsOnNode(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraphNode* Node) const override;
	virtual int32 GetNodeSelectionCount(const UEdGraph* Graph) const override;
	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;
	//~ End EdGraphSchema Interface

private:
	/** Adds actions for creating every type of UMoveNode */
	void GetAllMoveNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, bool bShowSelectedActions) const;
	/** Adds action for creating a comment */
	void GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph = nullptr) const;

private:
	/** Generates a list of all available UMoveNode classes */
	static void InitMoveNodeClasses();

	/** A list of all available MoveNode classes */
	static TArray<UClass*> MoveNodeClasses;
	/** Whether the list of MoveNode classes has been populated */
	static bool bMoveNodeClassesInitialized;
};

