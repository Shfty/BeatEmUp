// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SGraphNode.h"
#include "SMovesetGraphNode_Base.h"
#include "SMovesetGraphNode_Move.h"
#include "SMovesetGraphNode_Redirect.h"
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraph/MovesetGraphNode_Move.h"
#include "MovesetGraph/MovesetGraphNode_Redirect.h"


class FMovesetGraphNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* InNode) const override
	{
		if(UMovesetGraphNode_Move* MoveNode = Cast<UMovesetGraphNode_Move>(InNode))
		{
			return SNew(SMovesetGraphNode_Move, MoveNode);
		}

		if(UMovesetGraphNode_Redirect* RedirectNode = Cast<UMovesetGraphNode_Redirect>(InNode))
		{
			return SNew(SMovesetGraphNode_Redirect, RedirectNode);
		}

		if(UMovesetGraphNode_Base* BaseNode = Cast<UMovesetGraphNode_Base>(InNode))
		{
			return SNew(SMovesetGraphNode_Base, BaseNode);
		}

		return nullptr;
	}
};
