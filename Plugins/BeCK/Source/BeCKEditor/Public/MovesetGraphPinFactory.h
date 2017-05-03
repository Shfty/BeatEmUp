// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BeCKEditor.h"
#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "MovesetGraph/MovesetGraphSchema.h"
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraph/MovesetGraphNode_Wildcard.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SGraphNode.h"
#include "SMovesetGraphPin_Exec.h"
#include "SMovesetGraphPin_Input.h"
#include "SMovesetGraphPin_RedirectTarget.h"


class FMovesetGraphPinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		if(const UMovesetGraphSchema* MovesetSchema = Cast<const UMovesetGraphSchema>(InPin->GetSchema()))
		{
			if(InPin->PinType.PinCategory == MovesetSchema->PC_Exec)
			{
				return SNew(SMovesetGraphPin_Exec, InPin);
			}

			if(InPin->PinType.PinCategory == MovesetSchema->PC_Byte)
			{
				// Check for valid enum object reference
				if((InPin->PinType.PinSubCategoryObject != NULL) && (InPin->PinType.PinSubCategoryObject->IsA(UEnum::StaticClass())))
				{
					return SNew(SMovesetGraphPin_Input, InPin);
				}
			}

			if(InPin->PinType.PinCategory == MovesetSchema->PC_RedirectTarget)
			{
				return SNew(SMovesetGraphPin_RedirectTarget, InPin);
			}
		}

		return nullptr;
	}
};
