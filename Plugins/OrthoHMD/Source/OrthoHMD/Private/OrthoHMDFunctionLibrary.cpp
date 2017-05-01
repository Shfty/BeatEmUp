// Fill out your copyright notice in the Description page of Project Settings.

#include "OrthoHMD.h"
#include "OrthoHMDFunctionLibrary.h"


void UOrthoHMDFunctionLibrary::SetShearFactor(const FVector2D& InShearFactor)
{
	FOrthoHMD::ShearFactor = InShearFactor;
}

void UOrthoHMDFunctionLibrary::SetShearOffset(const FVector2D& InShearOffset)
{
	FOrthoHMD::ShearOffset = InShearOffset;
}

FVector2D& UOrthoHMDFunctionLibrary::GetShearFactor()
{
	return FOrthoHMD::ShearFactor;
}

FVector2D& UOrthoHMDFunctionLibrary::GetShearOffset()
{
	return FOrthoHMD::ShearOffset;
}