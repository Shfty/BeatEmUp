// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.


#include "BeCKEditor.h"
#include "SMovesetGraphPin_Exec.h"
#include "KismetPins/SGraphPinExec.h"
#include "Widgets/Layout/SSpacer.h"

void SMovesetGraphPin_Exec::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

	bWasEventPin = false;

	CachedImg_Pin_ConnectedHovered = FEditorStyle::GetBrush(TEXT("Graph.ExecPin.ConnectedHovered"));
	CachedImg_Pin_Connected = FEditorStyle::GetBrush(TEXT("Graph.ExecPin.Connected"));
	CachedImg_Pin_DisconnectedHovered = FEditorStyle::GetBrush(TEXT("Graph.ExecPin.DisconnectedHovered"));
	CachedImg_Pin_Disconnected = FEditorStyle::GetBrush(TEXT("Graph.ExecPin.Disconnected"));
}

TSharedRef<SWidget>	SMovesetGraphPin_Exec::GetDefaultValueWidget()
{
	return SNew(SSpacer); // not used for exec pin		
}

const FSlateBrush* SMovesetGraphPin_Exec::GetPinIcon() const
{
	const FSlateBrush* Brush = NULL;

	if(IsConnected())
	{
		Brush = IsHovered() ? CachedImg_Pin_ConnectedHovered : CachedImg_Pin_Connected;
	}
	else
	{
		Brush = IsHovered() ? CachedImg_Pin_DisconnectedHovered : CachedImg_Pin_Disconnected;
	}

	return Brush;
}
