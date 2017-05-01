// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUp.h"

#include "MovesetNode_Event.h"

UMovesetNode_Event::UMovesetNode_Event(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

#if WITH_EDITOR

FText UMovesetNode_Event::GetTitle() const
{
	return EventName != NAME_None ? FText::FromString(TEXT("Event ") + EventName.ToString()) : FText::FromString(TEXT("Event"));
}

#endif
