// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Move.h"
#include "Object.h"
#include "MovesetNode_Move.h"
#include "Moveset.generated.h"

class UMoveset;
class UMovesetInputSchema;
class UMovesetNode_Base;

#if WITH_EDITOR
class UEdGraph;
class UEdGraphNode;

/** Interface for moveset graph interaction with the MovesetEditor module. */
class IMovesetNodeEditor
{
public:
	virtual ~IMovesetNodeEditor() {};

	/** Called when creating a new sound cue graph. */
	virtual UEdGraph* CreateNewMovesetGraph(UMoveset* InMoveset) = 0;

	/** Sets up a moveset node. */
	virtual void SetupMovesetNode(UEdGraph* MovesetGraph, UMovesetNode_Base* MoveNode, bool bSelectNewNode) = 0;

	/** Links graph nodes from sound nodes. */
	virtual void LinkGraphNodesFromMoveNodes(UMoveset* Moveset) = 0;

	/** Compiles sound nodes from graph nodes. */
	virtual void CompileMoveNodesFromGraphNodes(UMoveset* Moveset) = 0;

	/** Removes nodes which are null from the sound cue graph. */
	virtual void RemoveNullNodes(UMoveset* Moveset) = 0;

	/** Renames all pins in a sound cue node */
	virtual void RenameNodePins(UMovesetNode_Base* MoveNode) = 0;
};
#endif

UCLASS(hidecategories = object, MinimalAPI, BlueprintType)
class UMoveset : public UObject
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void PostInitProperties() override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
#endif

	/** Construct and initialize a node within this Cue */
	template<class T>
	T* ConstructMoveNode(TSubclassOf<UMovesetNode_Base> MoveNodeClass = T::StaticClass(), bool bSelectNewNode = true)
	{
		// Set flag to be transactional so it registers with undo system
		T* MovesetNode = NewObject<T>(this, MoveNodeClass, NAME_None, RF_Transactional);

		UMovesetNode_Move* MoveNode = Cast<UMovesetNode_Move>(MovesetNode);
		if(MoveNode != nullptr)
		{
			MoveNode->Move = NewObject<UMove>(MoveNode, UMove::StaticClass(), NAME_None, RF_Transactional);
		}

#if WITH_EDITOR
		AllNodes.Add(MovesetNode);
		SetupMovesetNode(MovesetNode, bSelectNewNode);
#endif // WITH_EDITORONLY_DATA
		return MovesetNode;
	}

#if WITH_EDITOR
	/** Create the basic sound graph */
	BEATEMUP_API void CreateGraph();

	/** Clears all nodes from the graph (for old editor's buffer soundcue) */
	BEATEMUP_API void ClearGraph();

	/** Set up EdGraph parts of a SoundNode */
	BEATEMUP_API void SetupMovesetNode(class UMovesetNode_Base* InMoveNode, bool bSelectNewNode = true);

	/** Use the SoundCue's children to link EdGraph Nodes together */
	BEATEMUP_API void LinkGraphNodesFromMoveNodes();

	/** Use the EdGraph representation to compile the SoundCue */
	BEATEMUP_API void CompileMoveNodesFromGraphNodes();

	/** Get the EdGraph of SoundNodes */
	BEATEMUP_API class UEdGraph* GetGraph();

	/** Sets the moveset graph editor implementation.* */
	static BEATEMUP_API void SetMovesetNodeEditor(TSharedPtr<IMovesetNodeEditor> InMovesetGraphEditor);

	/** Gets the moveset graph editor implementation. */
	static TSharedPtr<IMovesetNodeEditor> BEATEMUP_API GetMovesetNodeEditor();
#endif

public:
	UPROPERTY()
	class UMovesetNode_Base* FirstNode;

	UPROPERTY()
	class UMovesetNode_Base* WildcardNode;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TArray<UMovesetNode_Base*> AllNodes;

	UPROPERTY()
	class UEdGraph* MovesetGraph;
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TAssetPtr<UEnum> InputSchema;

#if WITH_EDITOR
private:
	/** Ptr to interface to moveset editor operations. */
	static BEATEMUP_API TSharedPtr<IMovesetNodeEditor> MovesetMoveEditor;
#endif
};
