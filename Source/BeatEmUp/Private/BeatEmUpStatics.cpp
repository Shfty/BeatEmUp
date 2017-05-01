// Fill out your copyright notice in the Description page of Project Settings.

#include "BeatEmUp.h"
#include "BeatEmUpStatics.h"

float UBeatEmUpStatics::GetPlatformTimeSeconds()
{
	return FPlatformTime::ToSeconds(FPlatformTime::Cycles());
}
