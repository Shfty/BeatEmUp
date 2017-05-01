// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MovesetNode_Base.h"
#include "MovesetNode_Redirect.generated.h"

UCLASS()
class BEATEMUP_API UMovesetNode_Redirect : public UMovesetNode_Base
{
	GENERATED_BODY()

public:
	UMovesetNode_Redirect(const FObjectInitializer& ObjectInitializer);

	virtual UMovesetNode_Base* GetRedirectTarget() const { return RedirectTarget; };

#if WITH_EDITOR
	virtual void SetRedirectTarget(UMovesetNode_Base* InRedirectTarget);
	virtual FText GetTitle() const override;
#endif

protected:
	UPROPERTY()
	UMovesetNode_Base* RedirectTarget;
};

