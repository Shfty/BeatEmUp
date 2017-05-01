// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraph.h"
#include "MovesetGraph.generated.h"

UCLASS(MinimalAPI)
class UMovesetGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

	/** Returns the SoundCue that contains this graph */
	class UMoveset* GetMoveset() const;
};

