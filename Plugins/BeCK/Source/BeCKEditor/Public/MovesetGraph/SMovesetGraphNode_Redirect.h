// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "MovesetGraph/SMovesetGraphNode_Base.h"
#include "MovesetGraph/MovesetGraphNode_Redirect.h"
#include "SComboBox.h"

class SMovesetGraphNode_Redirect : public SMovesetGraphNode_Base
{
public:
	SLATE_BEGIN_ARGS(SMovesetGraphNode_Redirect) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UMovesetGraphNode_Redirect* InNode);

protected:
	// SGraphNode interface
	virtual TSharedRef<SWidget> CreateCenterContentArea() override;
};
