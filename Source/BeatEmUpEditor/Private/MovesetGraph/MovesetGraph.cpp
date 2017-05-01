// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpEditor.h"

#include "MovesetGraph/MovesetGraph.h"
#include "MovesetGraph/MovesetGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Moveset.h"
#include "MovesetNode_Move.h"
#include "MovesetNode_Input.h"
#include "MovesetNode_Redirect.h"
#include "MovesetNode_RedirectTarget.h"
#include "MovesetNode_FrameData.h"
#include "MovesetNode_Event.h"
#include "MovesetGraph/MovesetGraphNode_Base.h"
#include "MovesetGraph/MovesetGraphNode_Root.h"
#include "MovesetGraph/MovesetGraphNode_Wildcard.h"
#include "MovesetGraph/MovesetGraphNode_Move.h"
#include "MovesetGraph/MovesetGraphNode_Input.h"
#include "MovesetGraph/MovesetGraphNode_Redirect.h"
#include "MovesetGraph/MovesetGraphNode_RedirectTarget.h"
#include "MovesetGraph/MovesetGraphNode_FrameData.h"
#include "MovesetGraph/MovesetGraphNode_Event.h"

class FMovesetNodeEditor : public IMovesetNodeEditor
{
public:
	FMovesetNodeEditor()
	{

	}

	~FMovesetNodeEditor()
	{
	}

	UEdGraph* CreateNewMovesetGraph(UMoveset* InMoveset) override
	{
		UMovesetGraph* MovesetGraph = CastChecked<UMovesetGraph>(FBlueprintEditorUtils::CreateNewGraph(InMoveset, NAME_None, UMovesetGraph::StaticClass(), UMovesetGraphSchema::StaticClass()));

		return Cast<UEdGraph>(MovesetGraph);
	}

	/** Construct and initialize a node within this Cue */
	template<class T>
	T* ConstructMovesetNode(TSubclassOf<UMovesetNode_Base> MoveNodeClass = T::StaticClass(), bool bSelectNewNode = true)
	{
		// Set flag to be transactional so it registers with undo system
		T* MoveNode = NewObject<T>(this, MoveNodeClass, NAME_None, RF_Transactional);
		MoveNode->Move = NewObject<UMove>(MoveNode, UMove::StaticClass(), NAME_None, RF_Transactional);
#if WITH_EDITOR
		AllNodes.Add(MoveNode);
		SetupMoveNode(MoveNode, bSelectNewNode);
#endif // WITH_EDITORONLY_DATA
		return MoveNode;
	}

	void SetupMovesetNode(UEdGraph* MovesetGraph, UMovesetNode_Base* InMovesetNode, bool bSelectNewNode) override
	{
		UMovesetNode_Move* MoveNode = Cast<UMovesetNode_Move>(InMovesetNode);
		if(MoveNode != nullptr)
		{
			FGraphNodeCreator<UMovesetGraphNode_Move> NodeCreator(*MovesetGraph);
			UMovesetGraphNode_Move* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
			GraphNode->SetMovesetNode(InMovesetNode);
			NodeCreator.Finalize();
			return;
		}

		UMovesetNode_Input* InputNode = Cast<UMovesetNode_Input>(InMovesetNode);
		if(InputNode != nullptr)
		{
			FGraphNodeCreator<UMovesetGraphNode_Input> NodeCreator(*MovesetGraph);
			UMovesetGraphNode_Input* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
			GraphNode->SetMovesetNode(InMovesetNode);
			NodeCreator.Finalize();
			return;
		}

		UMovesetNode_Redirect* RedirectNode = Cast<UMovesetNode_Redirect>(InMovesetNode);
		if(RedirectNode != nullptr)
		{
			FGraphNodeCreator<UMovesetGraphNode_Redirect> NodeCreator(*MovesetGraph);
			UMovesetGraphNode_Redirect* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
			GraphNode->SetMovesetNode(InMovesetNode);
			NodeCreator.Finalize();
			return;
		}

		UMovesetNode_RedirectTarget* RedirectTargetNode = Cast<UMovesetNode_RedirectTarget>(InMovesetNode);
		if(RedirectTargetNode != nullptr)
		{
			FGraphNodeCreator<UMovesetGraphNode_RedirectTarget> NodeCreator(*MovesetGraph);
			UMovesetGraphNode_RedirectTarget* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
			GraphNode->SetMovesetNode(InMovesetNode);
			NodeCreator.Finalize();
			return;
		}

		UMovesetNode_FrameData* FrameDataNode = Cast<UMovesetNode_FrameData>(InMovesetNode);
		if(FrameDataNode != nullptr)
		{
			FGraphNodeCreator<UMovesetGraphNode_FrameData> NodeCreator(*MovesetGraph);
			UMovesetGraphNode_FrameData* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
			GraphNode->SetMovesetNode(InMovesetNode);
			NodeCreator.Finalize();
			return;
		}

		UMovesetNode_Event* EventNode = Cast<UMovesetNode_Event>(InMovesetNode);
		if(EventNode != nullptr)
		{
			FGraphNodeCreator<UMovesetGraphNode_Event> NodeCreator(*MovesetGraph);
			UMovesetGraphNode_Event* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
			GraphNode->SetMovesetNode(InMovesetNode);
			NodeCreator.Finalize();
			return;
		}

		FGraphNodeCreator<UMovesetGraphNode_Base> NodeCreator(*MovesetGraph);
		UMovesetGraphNode_Base* GraphNode = NodeCreator.CreateNode(bSelectNewNode);
		GraphNode->SetMovesetNode(InMovesetNode);
		NodeCreator.Finalize();
	}

	void LinkGraphNodesFromMoveNodes(UMoveset* Moveset) override
	{
		for(TArray<UMovesetNode_Base*>::TConstIterator It(Moveset->AllNodes); It; ++It)
		{
			UMovesetNode_Base* MoveNode = *It;
			if(MoveNode)
			{
				TArray<UEdGraphPin*> InputPins;
				CastChecked<UMovesetGraphNode_Base>(MoveNode->GetGraphNode())->GetInputPins(InputPins);
				check(InputPins.Num() == MoveNode->ChildNodes.Num());

				for(int32 ChildIndex = 0; ChildIndex < MoveNode->ChildNodes.Num(); ChildIndex++)
				{
					UMovesetNode_Base* ChildNode = MoveNode->ChildNodes[ChildIndex];
					if(ChildNode)
					{
						InputPins[ChildIndex]->BreakAllPinLinks();
						InputPins[ChildIndex]->MakeLinkTo(CastChecked<UMovesetGraphNode_Base>(ChildNode->GetGraphNode())->GetOutputPin(ChildIndex));
					}
				}
			}
		}
	}

	void CompileMoveNodesFromGraphNodes(UMoveset* Moveset) override
	{
		TArray<UMovesetNode_Base*> ChildNodes;

		TArray<UEdGraphPin*> InputPins;
		TArray<UEdGraphPin*> OutputPins;

		for(int32 NodeIndex = 0; NodeIndex < Moveset->MovesetGraph->Nodes.Num(); ++NodeIndex)
		{
			UMovesetGraphNode_Base* GraphNode = Cast<UMovesetGraphNode_Base>(Moveset->MovesetGraph->Nodes[NodeIndex]);
			if(GraphNode && GraphNode->MovesetNode)
			{
				// Node names
				GraphNode->MovesetNode->NodeName = FName(*GraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString());

				// Child nodes
				GraphNode->GetInputPins(InputPins);
				ChildNodes.Empty();
				for(int32 PinIndex = 0; PinIndex < InputPins.Num(); ++PinIndex)
				{
					UEdGraphPin* ChildPin = InputPins[PinIndex];

					if(ChildPin->LinkedTo.Num() > 0 && ChildPin->PinType.PinCategory != UMovesetGraphSchema::PC_Exec)
					{
						UMovesetGraphNode_Base* GraphChildNode = CastChecked<UMovesetGraphNode_Base>(ChildPin->LinkedTo[0]->GetOwningNode());
						ChildNodes.Add(GraphChildNode->MovesetNode);
					}
					else
					{
						ChildNodes.AddZeroed();
					}
				}

				GraphNode->MovesetNode->SetFlags(RF_Transactional);
				GraphNode->MovesetNode->Modify();
				GraphNode->MovesetNode->SetChildNodes(ChildNodes);
				GraphNode->MovesetNode->PostEditChange();

				// Output nodes
				GraphNode->GetOutputPins(OutputPins);

				if(OutputPins.Num() > 0)
				{
					UMovesetGraphNode_Move* MoveNode = Cast<UMovesetGraphNode_Move>(GraphNode);
					if(MoveNode != nullptr)
					{
						UMovesetGraphNode_Base* UpdateNode = nullptr;
						UMovesetGraphNode_Base* FinishNode = nullptr;
						TArray<UMovesetNode_Base*> LinkNodes;

						for(int32 i = 0; i < OutputPins.Num(); ++i)
						{
							UEdGraphPin* Pin = OutputPins[i];

							switch(i)
							{
								case 0:
									if(Pin->LinkedTo.Num() > 0 && Pin->LinkedTo[0] != nullptr)
									{
										UpdateNode = CastChecked<UMovesetGraphNode_Base>(Pin->LinkedTo[0]->GetOwningNode());
									}
									break;
								case 1:
									if(Pin->LinkedTo.Num() > 0 && Pin->LinkedTo[0] != nullptr)
									{
										FinishNode = CastChecked<UMovesetGraphNode_Base>(Pin->LinkedTo[0]->GetOwningNode());
									}
									break;
								default:
									LinkNodes.Add(nullptr);
									if(Pin->LinkedTo.Num() > 0 && Pin->LinkedTo[0] != nullptr)
									{
										UMovesetGraphNode_Base* LinkNode = CastChecked<UMovesetGraphNode_Base>(Pin->LinkedTo[0]->GetOwningNode());
										if(LinkNode != nullptr)
										{
											LinkNodes[LinkNodes.Num() - 1] = LinkNode->MovesetNode;
										}
									}
									break;
							}
						}

						UMovesetNode_Move* MovesetNode = Cast<UMovesetNode_Move>(MoveNode->MovesetNode);
						if(MovesetNode != nullptr)
						{
							MovesetNode->SetFlags(RF_Transactional);
							MovesetNode->Modify();
							MovesetNode->SetUpdateNode(UpdateNode != nullptr ? UpdateNode->MovesetNode : nullptr);
							MovesetNode->SetFinishNode(FinishNode != nullptr ? FinishNode->MovesetNode : nullptr);
							MovesetNode->SetLinkNodes(LinkNodes);
							MovesetNode->PostEditChange();
						}
					}

					UMovesetGraphNode_Input* InputNode = Cast<UMovesetGraphNode_Input>(GraphNode);
					if(InputNode != nullptr)
					{
						UMovesetGraphNode_Base* PressedNode = nullptr;
						UEdGraphPin* PressedPin = OutputPins[0];
						if(PressedPin->LinkedTo.Num() > 0 && PressedPin->LinkedTo[0] != nullptr)
						{
							PressedNode = CastChecked<UMovesetGraphNode_Base>(PressedPin->LinkedTo[0]->GetOwningNode());
						}

						UMovesetGraphNode_Base* ReleasedNode = nullptr;
						UEdGraphPin* ReleasedPin = OutputPins[1];
						if(ReleasedPin->LinkedTo.Num() > 0 && ReleasedPin->LinkedTo[0] != nullptr)
						{
							ReleasedNode = CastChecked<UMovesetGraphNode_Base>(ReleasedPin->LinkedTo[0]->GetOwningNode());
						}

						UMovesetGraphNode_Base* TrueNode = nullptr;
						UEdGraphPin* TruePin = OutputPins[2];
						if(TruePin->LinkedTo.Num() > 0 && TruePin->LinkedTo[0] != nullptr)
						{
							TrueNode = CastChecked<UMovesetGraphNode_Base>(TruePin->LinkedTo[0]->GetOwningNode());
						}

						UMovesetGraphNode_Base* FalseNode = nullptr;
						UEdGraphPin* FalsePin = OutputPins[3];
						if(FalsePin->LinkedTo.Num() > 0 && FalsePin->LinkedTo[0] != nullptr)
						{
							FalseNode = CastChecked<UMovesetGraphNode_Base>(FalsePin->LinkedTo[0]->GetOwningNode());
						}

						UMovesetNode_Input* MovesetNode = Cast<UMovesetNode_Input>(InputNode->MovesetNode);
						if(MovesetNode != nullptr)
						{
							MovesetNode->SetFlags(RF_Transactional);
							MovesetNode->Modify();
							MovesetNode->SetPressedNode(PressedNode != nullptr ? PressedNode->MovesetNode : nullptr);
							MovesetNode->SetReleasedNode(ReleasedNode != nullptr ? ReleasedNode->MovesetNode : nullptr);
							MovesetNode->SetTrueNode(TrueNode != nullptr ? TrueNode->MovesetNode : nullptr);
							MovesetNode->SetFalseNode(FalseNode != nullptr ? FalseNode->MovesetNode : nullptr);
							MovesetNode->PostEditChange();
						}
					}

					UMovesetGraphNode_Redirect* RedirectNode = Cast<UMovesetGraphNode_Redirect>(GraphNode);
					if(RedirectNode != nullptr)
					{
						TArray<UMovesetGraphNode_RedirectTarget*> RedirectTargets;
						Moveset->MovesetGraph->GetNodesOfClass<UMovesetGraphNode_RedirectTarget>(RedirectTargets);

						FString TargetName;
						UEdGraphPin* TargetPin = RedirectNode->GetInputPin(1);
						if(TargetPin != nullptr)
						{
							TargetName = TargetPin->DefaultValue;
						}

						UMovesetGraphNode_RedirectTarget** RedirectTargetPtr = RedirectTargets.FindByPredicate(
							[TargetName](const UMovesetGraphNode_RedirectTarget* ExistingRedirectTarget) {
							return ExistingRedirectTarget->CustomTitle.ToString() == TargetName;
						});

						if(RedirectTargetPtr != nullptr)
						{
							UMovesetGraphNode_Base* RedirectTargetNode = *RedirectTargetPtr;
							if(RedirectTargetNode != nullptr)
							{
								UMovesetGraphNode_Base* RedirectTarget = nullptr;

								UEdGraphPin* RedirectOutputPin = RedirectTargetNode->GetOutputPin(0);
								if(RedirectOutputPin->LinkedTo.Num() > 0 && RedirectOutputPin->LinkedTo[0] != nullptr)
								{
									RedirectTarget = CastChecked<UMovesetGraphNode_Base>(RedirectOutputPin->LinkedTo[0]->GetOwningNode());
								}

								UMovesetNode_Redirect* MovesetNode = Cast<UMovesetNode_Redirect>(RedirectNode->MovesetNode);
								if(MovesetNode != nullptr)
								{
									MovesetNode->SetFlags(RF_Transactional);
									MovesetNode->Modify();
									MovesetNode->SetRedirectTarget(RedirectTarget != nullptr ? RedirectTarget->MovesetNode : nullptr);
									MovesetNode->PostEditChange();
								}
							}
						}
					}

					UMovesetGraphNode_Base* ExecNode = nullptr;
					UEdGraphPin* ExecPin = OutputPins[0];
					if(ExecPin->LinkedTo.Num() > 0 && ExecPin->LinkedTo[0] != nullptr)
					{
						ExecNode = CastChecked<UMovesetGraphNode_Base>(ExecPin->LinkedTo[0]->GetOwningNode());
					}

					GraphNode->MovesetNode->SetFlags(RF_Transactional);
					GraphNode->MovesetNode->Modify();
					GraphNode->MovesetNode->SetExecNode(ExecNode != nullptr ? ExecNode->MovesetNode : nullptr);
					GraphNode->MovesetNode->PostEditChange();
				}
			}
			else
			{
				// Set FirstNode based on RootNode connection
				UMovesetGraphNode_Root* RootNode = Cast<UMovesetGraphNode_Root>(Moveset->MovesetGraph->Nodes[NodeIndex]);
				if(RootNode)
				{
					Moveset->Modify();
					if(RootNode->Pins[0]->LinkedTo.Num() > 0)
					{
						Moveset->FirstNode = CastChecked<UMovesetGraphNode_Base>(RootNode->Pins[0]->LinkedTo[0]->GetOwningNode())->MovesetNode;
					}
					else
					{
						Moveset->FirstNode = nullptr;
					}
					Moveset->PostEditChange();
				}

				// Set WildcardNode based on RootNode connection
				UMovesetGraphNode_Wildcard* WildcardNode = Cast<UMovesetGraphNode_Wildcard>(Moveset->MovesetGraph->Nodes[NodeIndex]);
				if(WildcardNode)
				{
					Moveset->Modify();
					if(WildcardNode->Pins[0]->LinkedTo.Num() > 0)
					{
						Moveset->WildcardNode = CastChecked<UMovesetGraphNode_Base>(WildcardNode->Pins[0]->LinkedTo[0]->GetOwningNode())->MovesetNode;
					}
					else
					{
						Moveset->WildcardNode = nullptr;
					}
					Moveset->PostEditChange();
				}
			}
		}
	}

	void RemoveNullNodes(UMoveset* Moveset) override
	{
		// Deal with MovesetNode types being removed - iterate in reverse as nodes may be removed
		for(int32 idx = Moveset->MovesetGraph->Nodes.Num() - 1; idx >= 0; --idx)
		{
			UMovesetGraphNode_Base* Node = Cast<UMovesetGraphNode_Base>(Moveset->MovesetGraph->Nodes[idx]);

			if(Node && Node->MovesetNode == nullptr)
			{
				FBlueprintEditorUtils::RemoveNode(nullptr, Node, true);
			}
		}
	}

	void RenameNodePins(UMovesetNode_Base* MoveNode) override
	{
		TArray<class UEdGraphPin*> InputPins;

#if WITH_EDITORONLY_DATA
		CastChecked<UMovesetGraphNode_Base>(MoveNode->GetGraphNode())->GetInputPins(InputPins);
#endif

		for(int32 i = 0; i < InputPins.Num(); i++)
		{
			if(InputPins[i])
			{
				InputPins[i]->PinName = MoveNode->GetInputPinName(i).ToString();
			}
		}
	}
};


UMovesetGraph::UMovesetGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if(!UMoveset::GetMovesetNodeEditor().IsValid())
	{
		UMoveset::SetMovesetNodeEditor(TSharedPtr<IMovesetNodeEditor>(new FMovesetNodeEditor()));
	}
}

UMoveset* UMovesetGraph::GetMoveset() const
{
	return CastChecked<UMoveset>(GetOuter());
}
