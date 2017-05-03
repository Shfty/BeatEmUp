// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BeCKEditor.h"
#include "MovesetGraphNode_Redirect.h"
#include "MovesetGraph/MovesetGraphSchema.h"
#include "GraphEditorSettings.h"

#define LOCTEXT_NAMESPACE "MovesetGraphNode_Redirect"

UMovesetGraphNode_Redirect::UMovesetGraphNode_Redirect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FLinearColor UMovesetGraphNode_Redirect::GetNodeTitleColor() const
{
	
	return GetDefault<UGraphEditorSettings>()->PureFunctionCallNodeTitleColor;
}

void UMovesetGraphNode_Redirect::CreateInputPins()
{
	Super::CreateInputPins();

	CreatePin(EGPD_Input, UMovesetGraphSchema::PC_RedirectTarget, FString(), nullptr, false, false, TEXT("Target"));
}

#undef LOCTEXT_NAMESPACE
