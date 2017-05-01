// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

/*-----------------------------------------------------------------------------
FMovesetEditorCommands
-----------------------------------------------------------------------------*/

class FMovesetGraphEditorCommands : public TCommands<FMovesetGraphEditorCommands>
{
public:
	/** Constructor */
	FMovesetGraphEditorCommands()
		: TCommands<FMovesetGraphEditorCommands>("MovesetEditor", NSLOCTEXT("Contexts", "MovesetGraphEditor", "Moveset Graph Editor"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	/** Plays the SoundCue */
	/** Breaks the node input/output link */
	TSharedPtr<FUICommandInfo> BreakLink;

	/** Adds an input to the node */
	TSharedPtr<FUICommandInfo> AddInput;

	/** Removes an input from the node */
	TSharedPtr<FUICommandInfo> DeleteInput;

	/** Initialize commands */
	virtual void RegisterCommands() override;
};
