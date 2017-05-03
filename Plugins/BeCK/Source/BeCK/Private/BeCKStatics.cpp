// Fill out your copyright notice in the Description page of Project Settings.

#include "BeCK.h"
#include "BeCKStatics.h"

float UBeCKStatics::GetPlatformTimeSeconds()
{
	return FPlatformTime::ToSeconds(FPlatformTime::Cycles());
}
