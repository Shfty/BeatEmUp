// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeCK.h"

#include "MovesetNode_RedirectTarget.h"
#include "Move.h"

UMovesetNode_RedirectTarget::UMovesetNode_RedirectTarget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

#if WITH_EDITOR
FText UMovesetNode_RedirectTarget::GetTitle() const
{
	return FText::FromString(TEXT("Redirect Target"));
}
#endif
