#include "BeCK.h"

#include "MovesetNode_Base.h"
#include "EngineUtils.h"
#include "Move.h"

UMovesetNode_Base::UMovesetNode_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMovesetNode_Base::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if(Ar.UE4Ver() >= VER_UE4_COOKED_ASSETS_IN_EDITOR_SUPPORT)
	{
		FStripDataFlags StripFlags(Ar);
#if WITH_EDITORONLY_DATA
		if(!StripFlags.IsEditorDataStripped())
		{
			Ar << GraphNode;
		}
#endif
	}
#if WITH_EDITOR
	else
	{
		Ar << GraphNode;
	}
#endif
}

#if WITH_EDITOR
void UMovesetNode_Base::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UMovesetNode_Base* This = CastChecked<UMovesetNode_Base>(InThis);

	Collector.AddReferencedObject(This->GraphNode, This);

	Super::AddReferencedObjects(InThis, Collector);
}
#endif //WITH_EDITOR

#if WITH_EDITORONLY_DATA
UEdGraphNode* UMovesetNode_Base::GetGraphNode() const
{
	return GraphNode;
}
#endif

void UMovesetNode_Base::GetChildNodes(TArray<UMovesetNode_Base*>& MoveNodes)
{
	MoveNodes.Add(this);
	for(int32 i = 0; i < ChildNodes.Num(); ++i)
	{
		if(ChildNodes[i])
		{
			ChildNodes[i]->GetChildNodes(MoveNodes);
		}
	}
}

#if WITH_EDITOR
void UMovesetNode_Base::CreateStartingConnectors()
{
	int32 ConnectorsToMake = FMath::Max(0, GetMinChildNodes());
	while(ConnectorsToMake > 0)
	{
		InsertChildNode(ChildNodes.Num());
		--ConnectorsToMake;
	}
}

void UMovesetNode_Base::InsertChildNode(int32 Index)
{
	check(Index >= 0 && Index <= ChildNodes.Num());
	int32 MaxChildNodes = GetMaxChildNodes();
	if(MaxChildNodes > ChildNodes.Num())
	{
		ChildNodes.InsertZeroed(Index);
	}
}

void UMovesetNode_Base::RemoveChildNode(int32 Index)
{
	check(Index >= 0 && Index < ChildNodes.Num());
	int32 MinChildNodes = GetMinChildNodes();
	if(ChildNodes.Num() > MinChildNodes)
	{
		ChildNodes.RemoveAt(Index);
	}
}

void UMovesetNode_Base::SetChildNodes(TArray<UMovesetNode_Base*>& InChildNodes)
{
	int32 MaxChildNodes = GetMaxChildNodes();
	int32 MinChildNodes = GetMinChildNodes();
	if(MaxChildNodes >= InChildNodes.Num() && InChildNodes.Num() >= MinChildNodes)
	{
		ChildNodes = InChildNodes;
	}
}

void UMovesetNode_Base::SetExecNode(UMovesetNode_Base* InExecNode)
{
	ExecNode = InExecNode;
}

void UMovesetNode_Base::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	MarkPackageDirty();
}

void UMovesetNode_Base::PostLoad()
{
	Super::PostLoad();

	// Make sure move nodes are transactional (so they work with undo system)
	SetFlags(RF_Transactional);
}

FText UMovesetNode_Base::GetTitle() const
{
	return GetClass()->GetDisplayNameText();
}

void UMovesetNode_Base::PlaceNode(int32 NodeColumn, int32 NodeRow, int32 RowCount)
{
	GraphNode->NodePosX = (-150 * NodeColumn) - 100;
	GraphNode->NodePosY = (100 * NodeRow) - (50 * RowCount);
}
#endif //WITH_EDITOR
