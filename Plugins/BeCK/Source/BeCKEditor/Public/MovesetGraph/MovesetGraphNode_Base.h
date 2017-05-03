// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "EdGraph/EdGraphNode.h"
#include "MovesetGraphNode_Base.generated.h"

class UMovesetNode_Base;
class FNodetextCache;

UCLASS(MinimalAPI)
class UMovesetGraphNode_Base : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UMovesetGraphNode_Base(const FObjectInitializer& ObjectInitializer);

	/** Estimate the width of this Node from the length of its title */
	int32 EstimateNodeWidth() const;

	/** Create all of the output pins required */
	virtual void CreateOutputPins();

	/** Get the Output Pins */
	void GetOutputPins(TArray<class UEdGraphPin*>& OutOutputPins);
	/** Get a single Input Pin via its index */
	class UEdGraphPin* GetOutputPin(int32 OutputIndex);
	/** Get the current Output Pin count */
	int32 GetOutputCount() const;

	/** Create all of the input pins required */
	virtual void CreateInputPins();
	/** Get all of the Input Pins */
	void GetInputPins(TArray<class UEdGraphPin*>& OutInputPins);
	/** Get a single Input Pin via its index */
	class UEdGraphPin* GetInputPin(int32 InputIndex);
	/** Get the current Input Pin count */
	int32 GetInputCount() const;

	/** Is this the undeletable root node */
	virtual bool IsRootNode() const { return false; }


	void CreateInputPin();
	void AddInputPin();
	void RemoveInputPin(UEdGraphPin* InGraphPin);
	bool CanAddInputPin() const;

	/**
	* Handles inserting the node between the FromPin and what the FromPin was original connected to
	*
	* @param FromPin			The pin this node is being spawned from
	* @param NewLinkPin		The new pin the FromPin will connect to
	* @param OutNodeList		Any nodes that are modified will get added to this list for notification purposes
	*/
	void InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList);

	void SetMovesetNode(UMovesetNode_Base* InMoveNode);

	void PostCopyNode();
	void PostEditImport() override;
	void PrepareForCopying() override;

	// UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual void ReconstructNode() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	// End of UEdGraphNode interface.

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;
	// End of UEdGraphNode interface

	// UObject interface
	virtual void PostLoad() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	// End of UObject interface

private:
	/** Make sure the movenode is owned by the Moveset */
	void ResetMovesetNodeOwner();

public:
	UPROPERTY(Instanced)
	UMovesetNode_Base* MovesetNode;

	UPROPERTY()
	FName CustomTitle;
};
