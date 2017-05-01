// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MovesetNode_Base.h"
#include "MovesetNode_Input.generated.h"

UCLASS()
class BEATEMUP_API UMovesetNode_Input : public UMovesetNode_Base
{
	GENERATED_BODY()

public:
	UMovesetNode_Input(const FObjectInitializer& ObjectInitializer);

	virtual UMovesetNode_Base* GetPressedNode() const { return PressedNode; };
	virtual UMovesetNode_Base* GetReleasedNode() const { return ReleasedNode; };
	virtual UMovesetNode_Base* GetTrueNode() const { return TrueNode; };
	virtual UMovesetNode_Base* GetFalseNode() const { return FalseNode; };

#if WITH_EDITOR
	virtual void SetPressedNode(UMovesetNode_Base* InPressedNode);
	virtual void SetReleasedNode(UMovesetNode_Base* InReleasedNode);
	virtual void SetTrueNode(UMovesetNode_Base* InTrueNode);
	virtual void SetFalseNode(UMovesetNode_Base* InFalseNode);

	virtual FText GetTitle() const override;
#endif

	virtual int32 GetMinChildNodes() const override { return 0; };
	virtual int32 GetMaxChildNodes() const override { return 0; };

public:
	UPROPERTY()
	uint8 InputValue;

	UPROPERTY()
	UMovesetNode_Base* PressedNode;

	UPROPERTY()
	UMovesetNode_Base* ReleasedNode;

	UPROPERTY()
	UMovesetNode_Base* TrueNode;

	UPROPERTY()
	UMovesetNode_Base* FalseNode;

	UPROPERTY(EditAnywhere, Category = "Input")
	uint32 AutoReset : 1;
};

