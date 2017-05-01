// Fill out your copyright notice in the Description page of Project Settings.

#include "BeatEmUp.h"
#include "BeatEmUpPawn.h"

bool ABeatEmUpPawn::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if(IsNetworkRelevantFor(RealViewer, ViewTarget, SrcLocation))
	{
		return true;
	}

	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

bool ABeatEmUpPawn::IsNetworkRelevantFor_Implementation(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	return true;
}
