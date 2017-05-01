// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Moveset.h"

class FMenuBuilder;
class UMoveset;

class FAssetTypeActions_Moveset : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_Moveset", "BeatEmUp Moveset"); }
	virtual FColor GetTypeColor() const override { return FColor(0, 175, 255); }
	virtual UClass* GetSupportedClass() const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual bool CanFilter() override { return true; }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; };
};

UClass* FAssetTypeActions_Moveset::GetSupportedClass() const
{
	return UMoveset::StaticClass();
}

void FAssetTypeActions_Moveset::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto Movesets = GetTypedWeakObjectPtrs<UMoveset>(InObjects);
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
}

void FAssetTypeActions_Moveset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for(auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Moveset = Cast<UMoveset>(*ObjIt);
		if(Moveset != nullptr)
		{
			FBeatEmUpEditorModule* SandboxEditorModule = &FModuleManager::LoadModuleChecked<FBeatEmUpEditorModule>("BeatEmUpEditor");
			SandboxEditorModule->CreateMovesetEditor(Mode, EditWithinLevelEditor, Moveset);
		}
	}
}