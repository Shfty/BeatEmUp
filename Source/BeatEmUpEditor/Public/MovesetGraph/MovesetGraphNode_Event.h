// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraphNode_Event.generated.h"

UCLASS(MinimalAPI)
class UMovesetGraphNode_Event : public UMovesetGraphNode_Base
{
	GENERATED_UCLASS_BODY()

public:
	// UEdGraphNode interface
	virtual FLinearColor GetNodeTitleColor() const override;
};
