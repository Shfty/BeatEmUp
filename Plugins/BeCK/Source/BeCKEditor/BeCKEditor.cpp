// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BeCKEditor.h"

#include "Moveset.h"
#include "MovesetEditor.h"
#include "MovesetGraphNodeFactory.h"
#include "MovesetGraphPinFactory.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_Moveset.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "FBeCKEditorModule"

void FBeCKEditorModule::StartupModule()
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

void FBeCKEditorModule::ShutdownModule()
{
	// Unregister factories
	FEdGraphUtilities::UnregisterVisualNodeFactory(MovesetGraphNodeFactory);
	FEdGraphUtilities::UnregisterVisualPinFactory(MovesetGraphPinFactory);

	// Unregister asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.UnregisterAssetTypeActions(MovesetAssetActions.ToSharedRef());
}

TSharedRef<IMovesetEditor> FBeCKEditorModule::CreateMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMoveset* Moveset)
{
	TSharedRef<FMovesetEditor> NewMovesetEditor(new FMovesetEditor());
	NewMovesetEditor->InitMovesetEditor(Mode, InitToolkitHost, Moveset);
	return NewMovesetEditor;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBeCKEditorModule, BeCKEditor)