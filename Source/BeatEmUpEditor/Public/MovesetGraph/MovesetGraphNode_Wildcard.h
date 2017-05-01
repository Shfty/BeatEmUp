// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "MovesetGraphNode_Base.h"
#include "MovesetGraphNode_Wildcard.generated.h"

UCLASS()
class BEATEMUPEDITOR_API UMovesetGraphNode_Wildcard : public UMovesetGraphNode_Base
{
	GENERATED_BODY()

public:
	UMovesetGraphNode_Wildcard(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode interface
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool CanDuplicateNode() const override { return false; }
	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;

	// UMovesetGraphNode_Base interface
	virtual void CreateInputPins() override {};
	virtual void CreateOutputPins() override;
};
