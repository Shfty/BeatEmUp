// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
MovesetGraphNode_Base.cpp
=============================================================================*/

#include "BeatEmUpEditor.h"

#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/EditorEngine.h"
#include "GraphEditorActions.h"
#include "MovesetGraphEditorCommands.h"
#include "ScopedTransaction.h"
#include "Framework/Commands/GenericCommands.h"
#include "Engine/Font.h"
#include "MovesetGraph/MovesetGraph.h"
#include "MovesetGraph/MovesetGraphSchema.h"
#include "Moveset.h"
#include "MovesetNode_Base.h"

/////////////////////////////////////////////////////
// UMovesetGraphNode_Base

UMovesetGraphNode_Base::UMovesetGraphNode_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMovesetGraphNode_Base::CreateOutputPins()
{
	UEdGraphPin* NewPin = CreatePin(EGPD_Output, UMovesetGraphSchema::PC_Exec, TEXT(""), nullptr, /*bIsArray=*/ false, /*bIsReference=*/ false, TEXT("Exec Out"));
	NewPin->PinFriendlyName = FText::FromString(TEXT(" "));
}

void UMovesetGraphNode_Base::GetOutputPins(TArray<class UEdGraphPin*>& OutOutputPins)
{
	OutOutputPins.Empty();

	for(int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if(Pins[PinIndex]->Direction == EGPD_Output)
		{
			OutOutputPins.Add(Pins[PinIndex]);
		}
	}
}

UEdGraphPin* UMovesetGraphNode_Base::GetOutputPin(int32 OutputIndex)
{
	check(OutputIndex >= 0 && OutputIndex < GetOutputCount());

	for(int32 PinIndex = 0, FoundOutputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if(Pins[PinIndex]->Direction == EGPD_Output)
		{
			if(OutputIndex == FoundOutputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundOutputs++;
			}
		}
	}

	return nullptr;
}

int32 UMovesetGraphNode_Base::GetOutputCount() const
{
	int32 OutputCount = 0;

	for(int32 PinIndex = 0, FoundOutputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if(Pins[PinIndex]->Direction == EGPD_Output)
		{
			OutputCount++;
		}
	}

	return OutputCount;
}

void UMovesetGraphNode_Base::CreateInputPins()
{
	UEdGraphPin* NewPin = CreatePin(EGPD_Input, UMovesetGraphSchema::PC_Exec, TEXT(""), nullptr, false, false, TEXT("Exec In"));
	NewPin->PinFriendlyName = FText::FromString(TEXT(" "));
}

void UMovesetGraphNode_Base::GetInputPins(TArray<class UEdGraphPin*>& OutInputPins)
{
	OutInputPins.Empty();

	for(int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if(Pins[PinIndex]->Direction == EGPD_Input)
		{
			OutInputPins.Add(Pins[PinIndex]);
		}
	}
}

UEdGraphPin* UMovesetGraphNode_Base::GetInputPin(int32 InputIndex)
{
	check(InputIndex >= 0 && InputIndex < GetInputCount());

	for(int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if(Pins[PinIndex]->Direction == EGPD_Input)
		{
			if(InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}

	return nullptr;
}

int32 UMovesetGraphNode_Base::GetInputCount() const
{
	int32 InputCount = 0;

	for(int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if(Pins[PinIndex]->Direction == EGPD_Input)
		{
			InputCount++;
		}
	}

	return InputCount;
}

void UMovesetGraphNode_Base::CreateInputPin()
{
	UEdGraphPin* NewPin = CreatePin(EGPD_Input, UMovesetGraphSchema::PC_FrameData, TEXT(""), NULL, false, false, MovesetNode->GetInputPinName(GetInputCount()).ToString());
	if(NewPin->PinName.IsEmpty())
	{
		// Makes sure pin has a name for lookup purposes but user will never see it
		NewPin->PinName = CreateUniquePinName(TEXT("Input"));
		NewPin->PinFriendlyName = FText::FromString(TEXT(" "));
	}
}

void UMovesetGraphNode_Base::AddInputPin()
{
	const FScopedTransaction Transaction(NSLOCTEXT("MovesetGraphEditor", "MovesetEditorAddInput", "Add Moveset Input"));
	Modify();
	CreateInputPin();

	UMoveset* SoundCue = CastChecked<UMovesetGraph>(GetGraph())->GetMoveset();
	SoundCue->CompileMoveNodesFromGraphNodes();
	SoundCue->MarkPackageDirty();

	// Refresh the current graph, so the pins can be updated
	GetGraph()->NotifyGraphChanged();
}

void UMovesetGraphNode_Base::RemoveInputPin(UEdGraphPin* InGraphPin)
{
	const FScopedTransaction Transaction(NSLOCTEXT("MovesetGraphEditor", "MovesetEditorDeleteInput", "Delete Moveset Input"));
	Modify();

	TArray<class UEdGraphPin*> InputPins;
	GetInputPins(InputPins);

	for(int32 InputIndex = 0; InputIndex < InputPins.Num(); InputIndex++)
	{
		if(InGraphPin == InputPins[InputIndex])
		{
			InGraphPin->MarkPendingKill();
			Pins.Remove(InGraphPin);
			// also remove the SoundNode child node so ordering matches
			MovesetNode->Modify();
			MovesetNode->RemoveChildNode(InputIndex);
			break;
		}
	}

	UMoveset* SoundCue = CastChecked<UMovesetGraph>(GetGraph())->GetMoveset();
	SoundCue->CompileMoveNodesFromGraphNodes();
	SoundCue->MarkPackageDirty();

	// Refresh the current graph, so the pins can be updated
	GetGraph()->NotifyGraphChanged();
}

bool UMovesetGraphNode_Base::CanAddInputPin() const
{
	if(MovesetNode)
	{
		// Check if adding another input would exceed max child nodes.
		return MovesetNode->ChildNodes.Num() < MovesetNode->GetMaxChildNodes();
	}
	else
	{
		return false;
	}
}

void UMovesetGraphNode_Base::InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList)
{
	const UMovesetGraphSchema* Schema = CastChecked<UMovesetGraphSchema>(GetSchema());

	// The pin we are creating from already has a connection that needs to be broken. We want to "insert" the new node in between, so that the output of the new node is hooked up too
	UEdGraphPin* OldLinkedPin = FromPin->LinkedTo[0];
	check(OldLinkedPin);

	FromPin->BreakAllPinLinks();

	// Hook up the old linked pin to the first valid output pin on the new node
	for(int32 OutpinPinIdx = 0; OutpinPinIdx<Pins.Num(); OutpinPinIdx++)
	{
		UEdGraphPin* OutputExecPin = Pins[OutpinPinIdx];
		check(OutputExecPin);
		if(ECanCreateConnectionResponse::CONNECT_RESPONSE_MAKE == Schema->CanCreateConnection(OldLinkedPin, OutputExecPin).Response)
		{
			if(Schema->TryCreateConnection(OldLinkedPin, OutputExecPin))
			{
				OutNodeList.Add(OldLinkedPin->GetOwningNode());
				OutNodeList.Add(this);
			}
			break;
		}
	}

	if(Schema->TryCreateConnection(FromPin, NewLinkPin))
	{
		OutNodeList.Add(FromPin->GetOwningNode());
		OutNodeList.Add(this);
	}
}

void UMovesetGraphNode_Base::SetMovesetNode(UMovesetNode_Base* InMovesetNode)
{
	MovesetNode = InMovesetNode;
	InMovesetNode->GraphNode = this;
}

void UMovesetGraphNode_Base::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	CreateInputPins();
	CreateOutputPins();
}

void UMovesetGraphNode_Base::ReconstructNode()
{
	// Break any links to 'orphan' pins
	for(int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
	{
		UEdGraphPin* Pin = Pins[PinIndex];
		TArray<class UEdGraphPin*>& LinkedToRef = Pin->LinkedTo;
		for(int32 LinkIdx = 0; LinkIdx < LinkedToRef.Num(); LinkIdx++)
		{
			UEdGraphPin* OtherPin = LinkedToRef[LinkIdx];
			// If we are linked to a pin that its owner doesn't know about, break that link
			if(!OtherPin->GetOwningNode()->Pins.Contains(OtherPin))
			{
				Pin->LinkedTo.Remove(OtherPin);
			}
		}
	}

	// Store the old Input and Output pins
	TArray<UEdGraphPin*> OldInputPins;
	GetInputPins(OldInputPins);
	TArray<UEdGraphPin*> OldOutputPins;
	GetOutputPins(OldOutputPins);

	// Move the existing pins to a saved array
	TArray<UEdGraphPin*> OldPins(Pins);
	Pins.Empty();

	// Recreate the new pins
	AllocateDefaultPins();

	// Get new Input and Output pins
	TArray<UEdGraphPin*> NewInputPins;
	GetInputPins(NewInputPins);
	TArray<UEdGraphPin*> NewOutputPins;
	GetOutputPins(NewOutputPins);

	for(int32 PinIndex = 0; PinIndex < OldInputPins.Num(); PinIndex++)
	{
		if(PinIndex < NewInputPins.Num())
		{
			NewInputPins[PinIndex]->CopyPersistentDataFromOldPin(*OldInputPins[PinIndex]);
		}
	}

	for(int32 PinIndex = 0; PinIndex < OldOutputPins.Num(); PinIndex++)
	{
		if(PinIndex < NewOutputPins.Num())
		{
			NewOutputPins[PinIndex]->CopyPersistentDataFromOldPin(*OldOutputPins[PinIndex]);
		}
	}

	// Throw away the original pins
	for(int32 OldPinIndex = 0; OldPinIndex < OldPins.Num(); ++OldPinIndex)
	{
		UEdGraphPin* OldPin = OldPins[OldPinIndex];
		OldPin->Modify();
		OldPin->BreakAllPinLinks();

		UEdGraphNode::DestroyPin(OldPin);
	}
	OldPins.Empty();
}

void UMovesetGraphNode_Base::AutowireNewNode(UEdGraphPin* FromPin)
{
	if(FromPin != nullptr)
	{
		const UMovesetGraphSchema* Schema = CastChecked<UMovesetGraphSchema>(GetSchema());

		TSet<UEdGraphNode*> NodeList;

		// auto-connect from dragged pin to first compatible pin on the new node
		for(int32 i = 0; i<Pins.Num(); i++)
		{
			UEdGraphPin* Pin = Pins[i];
			check(Pin);
			FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);
			if(ECanCreateConnectionResponse::CONNECT_RESPONSE_MAKE == Response.Response)
			{
				if(Schema->TryCreateConnection(FromPin, Pin))
				{
					NodeList.Add(FromPin->GetOwningNode());
					NodeList.Add(this);
				}
				break;
			}
			else if(ECanCreateConnectionResponse::CONNECT_RESPONSE_BREAK_OTHERS_A == Response.Response)
			{
				InsertNewNode(FromPin, Pin, NodeList);
				break;
			}
		}

		// Send all nodes that received a new pin connection a notification
		for(auto It = NodeList.CreateConstIterator(); It; ++It)
		{
			UEdGraphNode* Node = (*It);
			Node->NodeConnectionListChanged();
		}
	}
}

int32 UMovesetGraphNode_Base::EstimateNodeWidth() const
{
	const int32 EstimatedCharWidth = 6;
	FString NodeTitle = GetNodeTitle(ENodeTitleType::FullTitle).ToString();
	UFont* Font = GetDefault<UEditorEngine>()->EditorFont;
	int32 Result = NodeTitle.Len()*EstimatedCharWidth;

	if(Font)
	{
		Result = Font->GetStringSize(*NodeTitle);
	}

	return Result;
}

bool UMovesetGraphNode_Base::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UMovesetGraphSchema::StaticClass());
}


void UMovesetGraphNode_Base::PostLoad()
{
	Super::PostLoad();

	// Fixup any MoveNode back pointers that may be out of date
	if(MovesetNode)
	{
		MovesetNode->GraphNode = this;
	}

	for(int32 Index = 0; Index < Pins.Num(); ++Index)
	{
		UEdGraphPin* Pin = Pins[Index];
		if(Pin->PinName.IsEmpty())
		{
			// Makes sure pin has a name for lookup purposes but user will never see it
			if(Pin->Direction == EGPD_Input)
			{
				Pin->PinName = CreateUniquePinName(TEXT("Input"));
			}
			else
			{
				Pin->PinName = CreateUniquePinName(TEXT("Output"));
			}
			Pin->PinFriendlyName = FText::FromString(TEXT(" "));
		}
	}
}

void UMovesetGraphNode_Base::PostCopyNode()
{
	// Make sure the MoveNode goes back to being owned by the Moveset after copying.
	ResetMovesetNodeOwner();
}

void UMovesetGraphNode_Base::PostEditImport()
{
	Super::PostEditImport();

	// Make sure this MoveNode is owned by the Moveset it's being pasted into.
	ResetMovesetNodeOwner();
}

void UMovesetGraphNode_Base::PrepareForCopying()
{
	if(MovesetNode)
	{
		// Temporarily take ownership of the MoveNode, so that it is not deleted when cutting
		MovesetNode->Rename(nullptr, this, REN_DontCreateRedirectors);
	}
}

void UMovesetGraphNode_Base::ResetMovesetNodeOwner()
{
	if(MovesetNode)
	{
		UMoveset* Moveset = CastChecked<UMovesetGraph>(GetGraph())->GetMoveset();

		if(MovesetNode->GetOuter() != Moveset)
		{
			// Ensures MoveNode is owned by the Moveset
			MovesetNode->Rename(nullptr, Moveset, REN_DontCreateRedirectors);
		}

		// Set up the back pointer for newly created sound nodes
		MovesetNode->GraphNode = this;
	}
}

FText UMovesetGraphNode_Base::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if(!CustomTitle.IsNone())
	{
		return FText::FromName(CustomTitle);
	}
	else if(MovesetNode)
	{
		return MovesetNode->GetTitle();
	}

	return Super::GetNodeTitle(TitleType);
}

void UMovesetGraphNode_Base::OnRenameNode(const FString& NewName)
{
	CustomTitle = *NewName;
}

void UMovesetGraphNode_Base::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	if(!bDuplicateForPIE)
	{
		CreateNewGuid();
	}
}

void UMovesetGraphNode_Base::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	if(Context.Pin)
	{
		// If on an input that can be deleted, show option
		if(Context.Pin->Direction == EGPD_Input && MovesetNode->ChildNodes.Num() > MovesetNode->GetMinChildNodes())
		{
			Context.MenuBuilder->AddMenuEntry(FMovesetGraphEditorCommands::Get().DeleteInput);
		}
	}
	if(Context.Node)
	{
		Context.MenuBuilder->BeginSection("MovesetGraphNodeEdit");
		{
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Rename);
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);
			Context.MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ReconstructNodes);
		}
		Context.MenuBuilder->EndSection();

		Context.MenuBuilder->BeginSection("MovesetGraphNodeAdd");
		{
			if(CanAddInputPin())
			{
				Context.MenuBuilder->AddMenuEntry(FMovesetGraphEditorCommands::Get().AddInput);
			}
		}
		Context.MenuBuilder->EndSection();
	}
}
