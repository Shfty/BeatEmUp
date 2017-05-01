// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModuleManager.h"
#include "Public/IMovesetEditor.h"

class FMovesetGraphNodeFactory;
class FMovesetGraphPinFactory;
class FAssetTypeActions_Moveset;

DECLARE_LOG_CATEGORY_EXTERN(BeatEmUpEditor, All, All)

class FBeatEmUpEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Creates a new moveset editor */
	virtual TSharedRef<IMovesetEditor> CreateMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMoveset* Moveset);

private:
	TSharedPtr<FMovesetGraphNodeFactory> MovesetGraphNodeFactory;
	TSharedPtr<FMovesetGraphPinFactory> MovesetGraphPinFactory;

	TSharedPtr<FAssetTypeActions_Moveset> MovesetAssetActions;
};
