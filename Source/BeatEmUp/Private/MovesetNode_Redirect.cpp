// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUp.h"

#include "MovesetNode_Redirect.h"
#include "Move.h"

UMovesetNode_Redirect::UMovesetNode_Redirect(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

#if WITH_EDITOR

void UMovesetNode_Redirect::SetRedirectTarget(UMovesetNode_Base* InRedirectTarget)
{
	RedirectTarget = InRedirectTarget;
}

FText UMovesetNode_Redirect::GetTitle() const
{
	return FText::FromString(TEXT("Redirect"));
}

#endif
