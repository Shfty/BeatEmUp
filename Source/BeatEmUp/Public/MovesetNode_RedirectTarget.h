// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MovesetNode_Base.h"
#include "MovesetNode_RedirectTarget.generated.h"

UCLASS()
class BEATEMUP_API UMovesetNode_RedirectTarget : public UMovesetNode_Base
{
	GENERATED_BODY()

public:
	UMovesetNode_RedirectTarget(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual FText GetTitle() const override;
#endif

public:

};

