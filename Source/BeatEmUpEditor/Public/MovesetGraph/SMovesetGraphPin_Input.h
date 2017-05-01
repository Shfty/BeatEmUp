// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphPin.h"
#include "SGraphPinComboBox.h"

class SMovesetGraphPin_Input : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SMovesetGraphPin_Input) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;

private:
	FString OnGetText() const;
	void GenerateComboBoxIndexes(TArray< TSharedPtr<int32> >& OutComboBoxIndexes);
	void ComboBoxSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo);
	FText OnGetFriendlyName(int32 EnumIndex);
	FText OnGetTooltip(int32 EnumIndex);

	TSharedPtr<class SPinComboBox> ComboBox;
};
