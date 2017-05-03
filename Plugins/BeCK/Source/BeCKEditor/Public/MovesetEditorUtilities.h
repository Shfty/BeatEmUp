// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IMovesetEditor;
class UEdGraph;
class UMoveset;
struct Rect;

//////////////////////////////////////////////////////////////////////////
// FSoundCueEditorUtilities

class BECKEDITOR_API FMovesetEditorUtilities
{
public:
	/** Can we paste to this graph? */
	static bool CanPasteNodes(const class UEdGraph* Graph);

	/** Perform paste on graph, at location */
	static void  PasteNodesHere(class UEdGraph* Graph, const FVector2D& Location);

	/** Adds flipbook nodes based on selected objects
	*
	* @param SelectedMoves List of selected PaperFlipbooks to create nodes for
	* @param Moveset The Moveset that the nodes will be part of
	* @param OutMoves Stores all created nodes
	* @param Location Position of first created node
	*/
	static void CreateFlipbookContainers(TArray<class UPaperFlipbook*>& SelectedMoves, class UMoveset* Moveset, TArray<class UMovesetNode_Base*>& OutMoves, FVector2D Location);

	/** Get the bounding area for the currently selected nodes
	*
	* @param Graph The Graph we are finding bounds for
	* @param Rect Final output bounding area, including padding
	* @param Padding An amount of padding to add to all sides of the bounds
	*
	* @return false if nothing is selected*/
	static bool GetBoundsForSelectedNodes(const UEdGraph* Graph, class FSlateRect& Rect, float Padding = 0.0f);

	/** Gets the number of nodes that are currently selected */
	static int32 GetNumberOfSelectedNodes(const UEdGraph* Graph);

	/** Get the currently selected set of nodes */
	static TSet<UObject*> GetSelectedNodes(const UEdGraph* Graph);

	static void RefreshGraphNodes(const UEdGraph* Graph);

private:
	/** Get ISoundCueEditor for given object, if it exists */
	static TSharedPtr<class IMovesetEditor> GetIMovesetEditorForObject(const UObject* ObjectToFocusOn);

	FMovesetEditorUtilities() {}
};
