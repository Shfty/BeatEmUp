// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MovesetNode_Base.h"
#include "MovesetNode_Event.generated.h"

UCLASS()
class BECK_API UMovesetNode_Event : public UMovesetNode_Base
{
	GENERATED_BODY()

public:
	UMovesetNode_Event(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual FText GetTitle() const override;
#endif

public:
	UPROPERTY(EditAnywhere, Category = Event)
	FName EventName;

	UPROPERTY(EditAnywhere, Category = Event)
	FVector EventLocation;

	UPROPERTY(EditAnywhere, Category = Event)
	FVector EventDirection;
};

