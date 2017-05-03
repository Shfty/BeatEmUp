// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperFlipbook.h"
#include "Object.h"
#include "Move.generated.h"

UCLASS(DefaultToInstanced, EditInlineNew, BlueprintType)
class BECK_API UMove : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperFlipbook* Flipbook;
};
