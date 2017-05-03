// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "MovesetGraphNode_Base.h"
#include "MovesetGraphNode_Move.generated.h"

UCLASS(MinimalAPI)
class UMovesetGraphNode_Move : public UMovesetGraphNode_Base
{
	GENERATED_BODY()

public:
	UMovesetGraphNode_Move(const FObjectInitializer& ObjectInitializer);

	// UEdGraphNode Interface
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;

	// UMovesetGraphNode_Base Interface
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;

public:
	UPROPERTY()
	bool FlipbookPlaying;

	UPROPERTY()
	float FlipbookPlaybackPosition;
};
