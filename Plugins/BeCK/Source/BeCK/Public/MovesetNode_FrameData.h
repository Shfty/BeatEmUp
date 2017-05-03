// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FrameData.h"

#include "MovesetNode_Base.h"
#include "MovesetNode_FrameData.generated.h"

UCLASS()
class BECK_API UMovesetNode_FrameData : public UMovesetNode_Base
{
	GENERATED_BODY()

public:
	UMovesetNode_FrameData(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual FText GetTitle() const override;
#endif

public:
	UPROPERTY(EditAnywhere, Instanced, Category = FrameData)
	UFrameData* FrameData;
};

