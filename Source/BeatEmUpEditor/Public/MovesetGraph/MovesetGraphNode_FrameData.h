// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraphNode_FrameData.generated.h"

UCLASS(MinimalAPI)
class UMovesetGraphNode_FrameData : public UMovesetGraphNode_Base
{
	GENERATED_UCLASS_BODY()

public:
	// UMovesetGraphNode_Base interface
	virtual void CreateInputPins() override {};
	virtual void CreateOutputPins() override;
};
