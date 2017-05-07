// Fill out your copyright notice in the Description page of Project Settings.

#include "PunchGame.h"
#include "BeatEmUpStatics.h"
#include "IPAddress.h"

#include "SocketSubsystem.h"

TArray<FString> UBeatEmUpStatics::GetAllMapNamesInFolder(FString Folder, bool Recursive)
{
	TArray<FString> MapFiles;

	if(Recursive)
	{
		IFileManager::Get().FindFilesRecursive(MapFiles, *(FPaths::GameContentDir() + Folder), TEXT("*.umap"), true, false, false);
	}
	else
	{
		IFileManager::Get().FindFiles(MapFiles, *(FPaths::GameContentDir() + Folder), TEXT("*.umap"));
	}

	for(int32 i = 0; i < MapFiles.Num(); i++)
	{
		// replace the whole directory string with only the name of the map

		MapFiles[i].Split("/", nullptr, &MapFiles[i], ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		MapFiles[i] = MapFiles[i].LeftChop(5);
	}

	return MapFiles;
}

TArray<FString> UBeatEmUpStatics::GetAllMapNames()
{
	return GetAllMapNamesInFolder("");
}

bool UBeatEmUpStatics::ValidateIPAddress(FString IPAddress)
{
	if(IPAddress.IsEmpty())
	{
		return false;
	}

	bool IsValid;

	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Addr->SetIp(*IPAddress, IsValid);

	return IsValid;
}
