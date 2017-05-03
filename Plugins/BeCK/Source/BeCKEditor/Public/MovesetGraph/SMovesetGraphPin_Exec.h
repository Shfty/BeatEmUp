// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphPin.h"

class SMovesetGraphPin_Exec : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SMovesetGraphPin_Exec) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual const FSlateBrush* GetPinIcon() const override;
	//~ End SGraphPin Interface

protected:
	mutable bool bWasEventPin;

	mutable const FSlateBrush* CachedImg_Pin_ConnectedHovered;
	mutable const FSlateBrush* CachedImg_Pin_DisconnectedHovered;
};
