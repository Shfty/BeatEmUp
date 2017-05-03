// Fill out your copyright notice in the Description page of Project Settings.

#include "BeCK.h"
#include "BeCKPawn.h"

bool ABeCKPawn::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if(IsNetworkRelevantFor(RealViewer, ViewTarget, SrcLocation))
	{
		return true;
	}

	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

bool ABeCKPawn::IsNetworkRelevantFor_Implementation(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	return true;
}
