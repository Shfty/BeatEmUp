
#include "BeCK.h"

#if WITH_EDITOR
#include "Moveset.h"
#include "EdGraphSchema_K2.h"
#include "MovesetNode_Base.h"

TSharedPtr<IMovesetNodeEditor> UMoveset::MovesetMoveEditor = nullptr;

void UMoveset::PostInitProperties()
{
	Super::PostInitProperties();
	if(!HasAnyFlags(RF_ClassDefaultObject | RF_NeedLoad))
	{
		CreateGraph();
	}
}

void UMoveset::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UMoveset* This = CastChecked<UMoveset>(InThis);

	Collector.AddReferencedObject(This->MovesetGraph, This);

	Super::AddReferencedObjects(InThis, Collector);
}

class UEdGraph* UMoveset::GetGraph()
{
	return MovesetGraph;
}

void UMoveset::CreateGraph()
{
	if(MovesetGraph == nullptr)
	{
		MovesetGraph = UMoveset::GetMovesetNodeEditor()->CreateNewMovesetGraph(this);
		MovesetGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = MovesetGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*MovesetGraph);
	}
}

void UMoveset::ClearGraph()
{
	if(MovesetGraph)
	{
		MovesetGraph->Nodes.Empty();
		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = MovesetGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*MovesetGraph);
	}
}

void UMoveset::SetupMovesetNode(UMovesetNode_Base* InMoveNode, bool bSelectNewNode/* = true*/)
{
	// Create the graph node
	check(InMoveNode->GraphNode == nullptr);

	UMoveset::GetMovesetNodeEditor()->SetupMovesetNode(MovesetGraph, InMoveNode, bSelectNewNode);
}

void UMoveset::LinkGraphNodesFromMoveNodes()
{
	UMoveset::GetMovesetNodeEditor()->LinkGraphNodesFromMoveNodes(this);
}

void UMoveset::CompileMoveNodesFromGraphNodes()
{
	UMoveset::GetMovesetNodeEditor()->CompileMoveNodesFromGraphNodes(this);
}

void UMoveset::SetMovesetNodeEditor(TSharedPtr<IMovesetNodeEditor> InMovesetMoveEditor)
{
	check(!MovesetMoveEditor.IsValid());
	MovesetMoveEditor = InMovesetMoveEditor;
}

TSharedPtr<IMovesetNodeEditor> UMoveset::GetMovesetNodeEditor()
{
	return MovesetMoveEditor;
}
#endif
