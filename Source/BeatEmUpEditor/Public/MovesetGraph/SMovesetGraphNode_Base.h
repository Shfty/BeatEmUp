// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphNode.h"
#include "MovesetGraph/MovesetGraphNode_Base.h"

class FAssetThumbnail;
class FAssetThumbnailPool;

class SMovesetGraphNode_Base : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SMovesetGraphNode_Base) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UMovesetGraphNode_Base* InNode);

protected:
	virtual TSharedRef<SWidget> CreateCenterContentArea() { return SNullWidget::NullWidget; };

	// SGraphNode interface
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	virtual FReply OnAddPin() override;

protected:
	UMovesetGraphNode_Base* MovesetNode;
};
