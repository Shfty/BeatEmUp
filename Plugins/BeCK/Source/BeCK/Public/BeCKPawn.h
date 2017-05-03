// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "BeCKPawn.generated.h"

UCLASS()
class BECK_API ABeCKPawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "BeCK Pawn")
	bool IsNetworkRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const;
	virtual bool IsNetworkRelevantFor_Implementation(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const;
};
