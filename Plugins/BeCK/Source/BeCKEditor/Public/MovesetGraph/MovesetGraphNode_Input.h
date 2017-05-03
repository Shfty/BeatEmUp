// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "MovesetGraphNode_Base.h"
#include "MovesetGraphNode_Input.generated.h"

UCLASS(MinimalAPI)
class UMovesetGraphNode_Input : public UMovesetGraphNode_Base
{
	GENERATED_BODY()

public:
	// UEdGraphNode Interface
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;

	// UMovesetGraphNode_Base Interface
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
};
