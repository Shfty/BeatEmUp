// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "MovesetGraphNode_RedirectTarget.h"
#include "MovesetGraphNode_Root.generated.h"

UCLASS()
class BEATEMUPEDITOR_API UMovesetGraphNode_Root : public UMovesetGraphNode_RedirectTarget
{
	GENERATED_BODY()

public:
	UMovesetGraphNode_Root(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool CanDuplicateNode() const override { return false; }
	// End of UEdGraphNode interface

	// UMovesetGraphNode_Base interface
	virtual bool IsRootNode() const override { return true; };
	// End of UMovesetGraphNode_Base interface
};
