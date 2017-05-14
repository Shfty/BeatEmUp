// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.


#include "BeCKEditor.h"
#include "SMovesetGraphPin_Input.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/SListView.h"
#include "ScopedTransaction.h"
#include "MovesetGraphNode_Base.h"
#include "MovesetNode_Input.h"
#include "Moveset.h"
#include "MovesetGraph.h"

void SMovesetGraphPin_Input::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget>	SMovesetGraphPin_Input::GetDefaultValueWidget()
{
	//Get list of enum indexes
	TArray< TSharedPtr<int32> > ComboItems;
	GenerateComboBoxIndexes(ComboItems);

	//Create widget
	return SAssignNew(ComboBox, SPinComboBox)
		.ComboItemList(ComboItems)
		.VisibleText(this, &SMovesetGraphPin_Input::OnGetText)
		.OnSelectionChanged(this, &SMovesetGraphPin_Input::ComboBoxSelectionChanged)
		.Visibility(this, &SMovesetGraphPin_Input::GetDefaultValueVisibility)
		.OnGetDisplayName(this, &SMovesetGraphPin_Input::OnGetFriendlyName)
		.OnGetTooltip(this, &SMovesetGraphPin_Input::OnGetTooltip);
}

FText SMovesetGraphPin_Input::OnGetFriendlyName(int32 EnumIndex)
{
	UEnum* EnumPtr = Cast<UEnum>(GraphPinObj->PinType.PinSubCategoryObject.Get());

	check(EnumPtr);
	check(EnumIndex < EnumPtr->NumEnums());

	FText EnumValueName = EnumPtr->GetDisplayNameText(EnumIndex);
	if(EnumValueName.IsEmpty())
	{
		EnumValueName = FText::FromString(EnumPtr->GetEnumName(EnumIndex));
	}

	return EnumValueName;
}

FText SMovesetGraphPin_Input::OnGetTooltip(int32 EnumIndex)
{
	UEnum* EnumPtr = Cast<UEnum>(GraphPinObj->PinType.PinSubCategoryObject.Get());

	check(EnumPtr);
	check(EnumIndex < EnumPtr->NumEnums());

	FText EnumValueTooltip = EnumPtr->GetToolTipText(EnumIndex);
	return EnumValueTooltip;
}

void SMovesetGraphPin_Input::ComboBoxSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type /*SelectInfo*/)
{
	UEnum* EnumPtr = Cast<UEnum>(GraphPinObj->PinType.PinSubCategoryObject.Get());
	check(EnumPtr);

	FString EnumSelectionString;
	if(NewSelection.IsValid())
	{
		check(*NewSelection < EnumPtr->NumEnums() - 1);
		EnumSelectionString = EnumPtr->GetEnumName(*NewSelection);
	}
	else
	{
		EnumSelectionString = FName(NAME_None).ToString();
	}


	if(GraphPinObj->GetDefaultAsString() != EnumSelectionString)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeEnumPinValue", "Change Enum Pin Value"));
		GraphPinObj->Modify();

		//Set new selection
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, EnumSelectionString);

		UMovesetGraphNode_Base* OwningNode = Cast<UMovesetGraphNode_Base>(GraphPinObj->GetOwningNode());

		if(OwningNode != nullptr)
		{
			UMovesetNode_Input* InputNode = Cast<UMovesetNode_Input>(OwningNode->MovesetNode);

			if(InputNode != nullptr)
			{
				InputNode->InputValue = *NewSelection;

				UMoveset* Moveset = CastChecked<UMovesetGraph>(OwningNode->GetGraph())->GetMoveset();
				Moveset->CompileMoveNodesFromGraphNodes();
				Moveset->MarkPackageDirty();
			}
		}
	}
}

FString SMovesetGraphPin_Input::OnGetText() const
{
	FString SelectedString = GraphPinObj->GetDefaultAsString();

	UEnum* EnumPtr = Cast<UEnum>(GraphPinObj->PinType.PinSubCategoryObject.Get());
	if(EnumPtr && EnumPtr->NumEnums())
	{
		const int32 MaxIndex = EnumPtr->NumEnums() - 1;
		for(int32 EnumIdx = 0; EnumIdx < MaxIndex; ++EnumIdx)
		{
			// Ignore hidden enum values
			if(!EnumPtr->HasMetaData(TEXT("Hidden"), EnumIdx))
			{
				if(SelectedString == EnumPtr->GetEnumName(EnumIdx))
				{
					FString EnumDisplayName = EnumPtr->GetDisplayNameText(EnumIdx).ToString();
					if(EnumDisplayName.Len() == 0)
					{
						return SelectedString;
					}
					else
					{
						return EnumDisplayName;
					}
				}
			}
		}

		if(SelectedString == EnumPtr->GetEnumName(MaxIndex))
		{
			return TEXT("(INVALID)");
		}

	}
	return SelectedString;
}

void SMovesetGraphPin_Input::GenerateComboBoxIndexes(TArray< TSharedPtr<int32> >& OutComboBoxIndexes)
{
	UEnum* EnumPtr = Cast<UEnum>(GraphPinObj->PinType.PinSubCategoryObject.Get());
	if(EnumPtr)
	{

		//NumEnums() - 1, because the last item in an enum is the _MAX item
		for(int32 EnumIndex = 0; EnumIndex < EnumPtr->NumEnums() - 1; ++EnumIndex)
		{
			// Ignore hidden enum values
			if(!EnumPtr->HasMetaData(TEXT("Hidden"), EnumIndex))
			{
				TSharedPtr<int32> EnumIdxPtr(new int32(EnumIndex));
				OutComboBoxIndexes.Add(EnumIdxPtr);
			}
		}
	}
}
