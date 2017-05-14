// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeCK.h"

#include "MovesetNode_Move.h"
#include "Move.h"


#if WITH_EDITOR

UMovesetNode_Move::UMovesetNode_Move(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	bShowInMovelist(true)
{

}

void UMovesetNode_Move::SetUpdateNode(UMovesetNode_Base* InUpdateNode)
{
	UpdateNode = InUpdateNode;
}

void UMovesetNode_Move::SetFinishNode(UMovesetNode_Base* InFinishNode)
{
	FinishNode = InFinishNode;
}

void UMovesetNode_Move::SetLinkNodes(TArray<UMovesetNode_Base*> InLinkNodes)
{
	LinkNodes = InLinkNodes;
}

FText UMovesetNode_Move::GetTitle() const
{
	if(Move != nullptr)
	{
		if(Move->Flipbook != nullptr)
		{
			return FText::FromString(Move->Flipbook->GetName());
		}
	}

	return Super::GetTitle();
}

#endif
