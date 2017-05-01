// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpEditor.h"

#include "Moveset.h"
#include "MovesetEditor.h"
#include "MovesetGraphNodeFactory.h"
#include "MovesetGraphPinFactory.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_Moveset.h"
#include "EdGraphUtilities.h"

DEFINE_LOG_CATEGORY(BeatEmUpEditor)

#define LOCTEXT_NAMESPACE "FBeatEmUpEditorModule"

void FBeatEmUpEditorModule::StartupModule()
{
	// Register factories
	MovesetGraphNodeFactory = MakeShareable(new FMovesetGraphNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(MovesetGraphNodeFactory);

	MovesetGraphPinFactory = MakeShareable(new FMovesetGraphPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(MovesetGraphPinFactory);

	// Register asset types
	MovesetAssetActions = MakeShareable(new FAssetTypeActions_Moveset());
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MovesetAssetActions.ToSharedRef());
}

void FBeatEmUpEditorModule::ShutdownModule()
{
	// Unregister factories
	FEdGraphUtilities::UnregisterVisualNodeFactory(MovesetGraphNodeFactory);
	FEdGraphUtilities::UnregisterVisualPinFactory(MovesetGraphPinFactory);

	// Unregister asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.UnregisterAssetTypeActions(MovesetAssetActions.ToSharedRef());
}

TSharedRef<IMovesetEditor> FBeatEmUpEditorModule::CreateMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMoveset* Moveset)
{
	TSharedRef<FMovesetEditor> NewMovesetEditor(new FMovesetEditor());
	NewMovesetEditor->InitMovesetEditor(Mode, InitToolkitHost, Moveset);
	return NewMovesetEditor;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBeatEmUpEditorModule, BeatEmUpEditor)