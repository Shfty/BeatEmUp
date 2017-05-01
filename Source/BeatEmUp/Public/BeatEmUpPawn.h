// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "BeatEmUpPawn.generated.h"

UCLASS()
class BEATEMUP_API ABeatEmUpPawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "BeatEmUp Pawn")
	bool IsNetworkRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const;
	virtual bool IsNetworkRelevantFor_Implementation(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const;
};
