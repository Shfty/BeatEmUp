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

TArray<FName> UBeatEmUpStatics::GetInputAxisNames()
{
	TArray<FName> AxisNames;

	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		InputSettings->GetAxisNames(AxisNames);
	}

	return AxisNames;
}

TArray<FName> UBeatEmUpStatics::GetInputActionNames()
{
	TArray<FName> AxisNames;

	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		InputSettings->GetActionNames(AxisNames);
	}

	return AxisNames;
}

TArray<FInputAxisKeyMapping> UBeatEmUpStatics::GetInputAxisMappings()
{
	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		return InputSettings->AxisMappings;
	}

	TArray<FInputAxisKeyMapping> Empty;
	return Empty;
}

TArray<FInputActionKeyMapping> UBeatEmUpStatics::GetInputActionMappings()
{
	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		return InputSettings->ActionMappings;
	}

	TArray<FInputActionKeyMapping> Empty;
	return Empty;
}

TArray<FKey> UBeatEmUpStatics::GetInputKeysForAxis(FName Axis)
{
	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	TArray<FKey> Keys;

	if(InputSettings != nullptr)
	{
		for(auto AxisMapping : InputSettings->AxisMappings)
		{
			if(AxisMapping.AxisName == Axis)
			{
				Keys.Add(AxisMapping.Key);
			}
		}
	}

	return Keys;
}

TArray<FKey> UBeatEmUpStatics::GetInputKeysForAction(FName Action)
{
	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	TArray<FKey> Keys;

	if(InputSettings != nullptr)
	{
		for(auto ActionMapping : InputSettings->ActionMappings)
		{
			if(ActionMapping.ActionName == Action)
			{
				Keys.Add(ActionMapping.Key);
			}
		}
	}

	return Keys;
}

void UBeatEmUpStatics::SetInputAxisMappings(TArray<FInputAxisKeyMapping> NewAxisMappings)
{
	UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		InputSettings->AxisMappings.Empty();
		InputSettings->AxisMappings.Append(NewAxisMappings);
		InputSettings->SaveConfig();
	}
}

void UBeatEmUpStatics::SetInputActionMappings(TArray<FInputActionKeyMapping> NewActionMappings)
{
	UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		InputSettings->ActionMappings.Empty();
		InputSettings->ActionMappings.Append(NewActionMappings);
		InputSettings->SaveConfig();
	}
}

void UBeatEmUpStatics::RemapInputAxis(FName Axis, FKey OldKey, FKey NewKey)
{
	if(Axis.IsNone())
	{
		return;
	}

	UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		for(auto& AxisMapping : InputSettings->AxisMappings)
		{
			if(AxisMapping.AxisName == Axis && AxisMapping.Key == OldKey)
			{
				AxisMapping.Key = NewKey;
			}
		}

		InputSettings->SaveConfig();
	}
}

void UBeatEmUpStatics::RemapInputAction(FName Action, FKey OldKey, FKey NewKey)
{
	if(Action.IsNone())
	{
		return;
	}

	UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		if(InputSettings->ActionMappings.FindByPredicate([Action, NewKey](FInputActionKeyMapping Mapping) { return Mapping.ActionName == Action && Mapping.Key == NewKey; }) != nullptr)
		{
			return;
		}

		TArray<FInputActionKeyMapping> InputActions;
		for(auto& ActionMapping : InputSettings->ActionMappings)
		{
			if(ActionMapping.ActionName == Action && ActionMapping.Key == OldKey)
			{
				ActionMapping.Key = NewKey;
			}
		}

		InputSettings->SaveConfig();
	}
}


void UBeatEmUpStatics::ClearInputAxisMappings()
{
	UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		InputSettings->AxisMappings.Empty();
		InputSettings->SaveConfig();
	}
}

void UBeatEmUpStatics::ClearInputActionMappings()
{
	UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();

	if(InputSettings != nullptr)
	{
		InputSettings->ActionMappings.Empty();
		InputSettings->SaveConfig();
	}
}

void UBeatEmUpStatics::ApplyInputMappings(APlayerController* PlayerController)
{
	PlayerController->PlayerInput->ForceRebuildingKeyMaps();
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
