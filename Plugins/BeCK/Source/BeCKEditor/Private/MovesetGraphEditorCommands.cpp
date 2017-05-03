// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeCKEditor.h"
#include "MovesetGraphEditorCommands.h"

#define LOCTEXT_NAMESPACE "MovesetEditorCommands"

void FMovesetGraphEditorCommands::RegisterCommands()
{
	UI_COMMAND(AddInput, "Add Input", "Adds an input to the node", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DeleteInput, "Delete Input", "Removes an input from the node", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
