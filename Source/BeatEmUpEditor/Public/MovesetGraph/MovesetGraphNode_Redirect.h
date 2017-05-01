// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraphNode_Redirect.generated.h"

UCLASS(MinimalAPI)
class UMovesetGraphNode_Redirect : public UMovesetGraphNode_Base
{
	GENERATED_UCLASS_BODY()

public:
	// UEdGraphNode interface
	virtual FLinearColor GetNodeTitleColor() const override;

	// UMovesetGraphNode_Base interface
	virtual void CreateInputPins() override;
};
