#include "BeCK.h"
#include "MovesetComponent.h"
#include "Moveset.h"
#include "PaperFlipbookPlayer.h"
#include "MovesetNode_Base.h"
#include "MovesetNode_Move.h"
#include "MovesetNode_Input.h"
#include "MovesetNode_Redirect.h"
#include "MovesetNode_FrameData.h"
#include "MovesetNode_Event.h"
#include "FrameData.h"
#include "Engine/ActorChannel.h"
#include "UnrealNetwork.h"

#define DEBUG_PRINT 0

UMovesetComponent::UMovesetComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;

}

void UMovesetComponent::InitializeComponent()
{
	Super::InitializeComponent();

	FlipbookPlayer = NewObject<UPaperFlipbookPlayer>(this, TEXT("FlipbookPlayer"));
	FlipbookPlayer->OnPlaybackPositionChanged.BindUObject(this, &UMovesetComponent::HandlePlaybackPositionChanged);
	FlipbookPlayer->OnSpriteChanged.BindUObject(this, &UMovesetComponent::HandleSpriteChanged);
	FlipbookPlayer->OnFrameChanged.BindUObject(this, &UMovesetComponent::HandleFrameChanged);
	FlipbookPlayer->OnPlaybackFinished.BindUObject(this, &UMovesetComponent::HandlePlaybackFinished);
}

UMovesetNode_Move* UMovesetComponent::GetCurrentMove() const
{
	if(CurrentMove != nullptr)
	{
		return CurrentMove;
	}

	return nullptr;
}

void UMovesetComponent::SetCurrentMove(UMovesetNode_Move* NewMove, bool FireEvents, bool bForce)
{
	if(CurrentMove != NewMove || bForce)
	{
		CurrentMove = NewMove;

		FlipbookPlayer->SetFlipbook(CurrentMove->Move->Flipbook);
		FlipbookPlayer->Looping = CurrentMove->GetFinishNode() == nullptr;

		if(FireEvents)
		{
			OnMoveChanged.Broadcast(CurrentMove);
		}
		FlipbookPlayer->PlayFromStart();
	}
}

bool UMovesetComponent::GetInput(uint8 Input) const
{
	if(InputState.CurrentState.IsValidIndex(Input))
	{
		return InputState.CurrentState[Input];
	}

	return false;
}

void UMovesetComponent::SetInput(uint8 Input, bool Value)
{
	if(InputState.CurrentState.IsValidIndex(Input) && InputState.CurrentState[Input] != Value)
	{
		InputState.CurrentState[Input] = Value;
	}
}

void UMovesetComponent::BeginPlay()
{
	if(Moveset == nullptr)
	{
		return;
	}

	UEnum* MovesetInputSchema = Moveset->InputSchema.LoadSynchronous();

	InputState.CurrentState.AddZeroed(MovesetInputSchema->GetMaxEnumValue());
	InputState.PrevState.AddZeroed(MovesetInputSchema->GetMaxEnumValue());
	PressedState.AddZeroed(MovesetInputSchema->GetMaxEnumValue());
	ReleasedState.AddZeroed(MovesetInputSchema->GetMaxEnumValue());
}

void UMovesetComponent::SetInputs(TArray<bool> NewInputs, TArray<bool> NewPrevInputs)
{
	InputState.CurrentState = NewInputs;
	InputState.PrevState = NewPrevInputs;
}

void UMovesetComponent::TickMoveset(float DeltaTime)
{
	if(Moveset == nullptr)
	{
		return;
	}

	if(PressedState.Num() != InputState.CurrentState.Num())
	{
		PressedState.SetNumZeroed(InputState.CurrentState.Num());
	}

	if(ReleasedState.Num() != InputState.CurrentState.Num())
	{
		ReleasedState.SetNumZeroed(InputState.CurrentState.Num());
	}

	// Calculate local input state
	for(int32 i = 0; i < InputState.CurrentState.Num(); ++i)
	{
		PressedState[i] = InputState.CurrentState[i] && !InputState.PrevState[i];
		ReleasedState[i] = !InputState.CurrentState[i] && InputState.PrevState[i];
	}

#if DEBUG_PRINT
	GEngine->ClearOnScreenDebugMessages();
#endif

	// If we don't have a current move, try to get one from the root node
	if(CurrentMove == nullptr)
	{
		UMovesetNode_Base* FirstNode = Moveset->FirstNode;
		if(FirstNode != nullptr)
		{
			UMovesetNode_Move* MoveNode = Cast<UMovesetNode_Move>(FirstNode);
			if(MoveNode != nullptr)
			{
				SetCurrentMove(MoveNode);
			}
		}
	}

	// If the current move is valid, evaluate it
	if(CurrentMove != nullptr)
	{
#if DEBUG_PRINT
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::White, FString::Printf(TEXT("%s"), *CurrentMove->NodeName.ToString()), false);
#endif

		// First, check update node
		bool EvaluateLinkNodes = true;

		UMovesetNode_Base* UpdateNode = CurrentMove->GetUpdateNode();
		if(UpdateNode != nullptr)
		{
			EvaluateLinkNodes = !EvaluateNode(0, UpdateNode);
		}

		if(EvaluateLinkNodes)
		{
			TArray<UMovesetNode_Base*> LinkNodes = CurrentMove->GetLinkNodes();
			if(LinkNodes.Num() > 0)
			{
#if DEBUG_PRINT
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::White, FString(TEXT("Check link nodes")), false);
#endif

				TArray<UMovesetNode_Base*> ChildNodes = CurrentMove->ChildNodes;
				for(auto ChildNode : ChildNodes)
				{
					UMovesetNode_FrameData* FrameDataNode = Cast<UMovesetNode_FrameData>(ChildNode);
					if(FrameDataNode != nullptr && FrameDataNode->FrameData != nullptr)
					{
						ULinkFrames* LinkFrames = Cast<ULinkFrames>(FrameDataNode->FrameData);
						if(LinkFrames != nullptr)
						{
							if(LinkFrames->LinkIndex < LinkNodes.Num())
							{
								int32 FlipbookFrame = FlipbookPlayer->GetPlaybackPositionInFrames();
								if(LinkFrames->ActivePeriod.Contains(FlipbookFrame))
								{
									EvaluateNode(0, LinkNodes[LinkFrames->LinkIndex]);
								}
							}
						}
					}
				}
			}
		}
	}

	if(FlipbookPlayer != nullptr)
	{
		FlipbookPlayer->TickFlipbook(DeltaTime);
#if DEBUG_PRINT
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::White, FString::Printf(TEXT("%d"), FlipbookPlayer->GetPlaybackPositionInFrames()), false);
#endif
	}
}

void UMovesetComponent::PostTickMoveset()
{
	for(int32 i = 0; i < InputState.CurrentState.Num() - 1; ++i)
	{
		InputState.PrevState[i] = InputState.CurrentState[i];
	}
}

void UMovesetComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UMovesetComponent::EvaluateNode(int TreeDepth, UMovesetNode_Base* Node)
{
	if(InputState.CurrentState.Num() > 0 && Node != nullptr)
	{
#if DEBUG_PRINT
		FString TabString;
		for(int i = 0; i < TreeDepth; ++i)
		{
			TabString.Append(TEXT("    "));
		}
		TabString.Append(TEXT("|-->"));

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::White, FString::Printf(TEXT("%s %s"), *TabString, *Node->NodeName.ToString()), false);
#endif

		UMovesetNode_Move* MoveNode = Cast<UMovesetNode_Move>(Node);
		if(MoveNode != nullptr)
		{
			SetCurrentMove(MoveNode, true, true);

			return true;
		}

		UMovesetNode_Input* InputNode = Cast<UMovesetNode_Input>(Node);
		if(InputNode != nullptr)
		{
			UMovesetNode_Base* PressedNode = InputNode->GetPressedNode();
			if(PressedNode != nullptr)
			{
				if(PressedState[InputNode->InputValue])
				{
					return EvaluateNode(TreeDepth + 1, PressedNode);
				}
			}  

			UMovesetNode_Base* ReleasedNode = InputNode->GetReleasedNode();
			if(ReleasedNode != nullptr)
			{
				if(ReleasedState[InputNode->InputValue])
				{
					return EvaluateNode(TreeDepth + 1, ReleasedNode);
				}
			}

			UMovesetNode_Base* TrueNode = InputNode->GetTrueNode();
			if(TrueNode != nullptr)
			{
				if(InputState.CurrentState[InputNode->InputValue])
				{
					return EvaluateNode(TreeDepth + 1, TrueNode);
				}
			}

			UMovesetNode_Base* FalseNode = InputNode->GetFalseNode();
			if(FalseNode != nullptr)
			{
				if(!InputState.CurrentState[InputNode->InputValue])
				{
					return EvaluateNode(TreeDepth + 1, FalseNode);
				}
			}
		}

		UMovesetNode_Redirect* RedirectNode = Cast<UMovesetNode_Redirect>(Node);
		if(RedirectNode != nullptr)
		{
			UMovesetNode_Base* RedirectTarget = RedirectNode->GetRedirectTarget();
			if(RedirectTarget != nullptr)
			{
				if(EvaluateNode(TreeDepth + 1, RedirectTarget))
				{
					return true;
				}
				else
				{
					UMovesetNode_Base* FallthroughNode = RedirectNode->GetExecNode();
					if(FallthroughNode != nullptr)
					{
						return EvaluateNode(TreeDepth, FallthroughNode);
					}
				}
			}
		}

		UMovesetNode_Event* EventNode = Cast<UMovesetNode_Event>(Node);
		if(EventNode != nullptr)
		{
			OnMovesetEvent.Broadcast(EventNode->EventName, EventNode->EventDirection, EventNode->EventDirection);

			UMovesetNode_Base* FallthroughNode = EventNode->GetExecNode();
			if(FallthroughNode != nullptr)
			{
				return EvaluateNode(TreeDepth, FallthroughNode);
			}
		}
	}

	return false;
}

void UMovesetComponent::HandlePlaybackPositionChanged(float NewPlaybackPosition)
{
	OnPlaybackPositionChanged.Broadcast(NewPlaybackPosition);
}

void UMovesetComponent::HandleSpriteChanged(int32 NewSpriteIndex)
{
	if(FlipbookPlayer != nullptr)
	{
		UPaperFlipbook* Flipbook = FlipbookPlayer->GetFlipbook();
		if(Flipbook != nullptr)
		{
			UPaperSprite* FlipbookSprite = Flipbook->GetSpriteAtFrame(NewSpriteIndex);
			OnSpriteChanged.Broadcast(FlipbookSprite);
		}
	}
}

void UMovesetComponent::HandleFrameChanged(int32 NewFrame)
{
	OnFrameChanged.Broadcast(NewFrame);
}

void UMovesetComponent::GetFrameData(TArray<UHitboxFrames*>& OutHitboxFrames,
													TArray<UArmorFrames*>& OutArmorFrames,
													TArray<UInvincibilityFrames*>& OutInvincibilityFrames,
													TArray<ULinkFrames*>& OutLinkFrames,
													TArray<UMotionFrames*>& OutMotionFrames,
													TArray<UImpulseFrames*>& OutImpulseFrames,
													TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames)
{
	TArray<UFrameData*> FrameData;
	TArray<UMovesetNode_Base*> ChildNodes = CurrentMove->ChildNodes;
	for(auto ChildNode : ChildNodes)
	{
		UMovesetNode_FrameData* FrameDataNode = Cast<UMovesetNode_FrameData>(ChildNode);
		if(FrameDataNode != nullptr && FrameDataNode->FrameData != nullptr)
		{
			FrameData.Add(FrameDataNode->FrameData);
		}
	}

	SortFrameDataByClass(FrameData, OutHitboxFrames, OutArmorFrames, OutInvincibilityFrames, OutLinkFrames, OutMotionFrames, OutImpulseFrames, OutMovementPropertyFrames);
}

void UMovesetComponent::GetFrameDataForFrame(int32 Frame,
											 TArray<UHitboxFrames*>& OutHitboxFrames,
											 TArray<UArmorFrames*>& OutArmorFrames,
											 TArray<UInvincibilityFrames*>& OutInvincibilityFrames,
											 TArray<ULinkFrames*>& OutLinkFrames,
											 TArray<UMotionFrames*>& OutMotionFrames,
											 TArray<UImpulseFrames*>& OutImpulseFrames,
											 TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames)
{
	if(CurrentMove != nullptr)
	{
		TArray<UFrameData*> FrameData;
		TArray<UMovesetNode_Base*> ChildNodes = CurrentMove->ChildNodes;
		for(auto ChildNode : ChildNodes)
		{
			UMovesetNode_FrameData* FrameDataNode = Cast<UMovesetNode_FrameData>(ChildNode);
			if(FrameDataNode != nullptr && FrameDataNode->FrameData != nullptr)
			{
				FInt32Interval ActivePeriod = FrameDataNode->FrameData->ActivePeriod;
				if(ActivePeriod.Contains(Frame))
				{
					FrameData.Add(FrameDataNode->FrameData);
				}
			}
		}

		SortFrameDataByClass(FrameData, OutHitboxFrames, OutArmorFrames, OutInvincibilityFrames, OutLinkFrames, OutMotionFrames, OutImpulseFrames, OutMovementPropertyFrames);
	}
}

void UMovesetComponent::GetFrameDataForCurrentFrame(TArray<UHitboxFrames*>& OutHitboxFrames, TArray<UArmorFrames*>& OutArmorFrames, TArray<UInvincibilityFrames*>& OutInvincibilityFrames, TArray<ULinkFrames*>& OutLinkFrames, TArray<UMotionFrames*>& OutMotionFrames, TArray<UImpulseFrames*>& OutImpulseFrames, TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames)
{
	return GetFrameDataForFrame(FlipbookPlayer->GetPlaybackPositionInFrames(),
								OutHitboxFrames,
								OutArmorFrames,
								OutInvincibilityFrames,
								OutLinkFrames,
								OutMotionFrames,
								OutImpulseFrames,
								OutMovementPropertyFrames);
}

void UMovesetComponent::SortFrameDataByClass(TArray<UFrameData*> FrameData,
											 TArray<UHitboxFrames*>& OutHitboxFrames,
											 TArray<UArmorFrames*>& OutArmorFrames,
											 TArray<UInvincibilityFrames*>& OutInvincibilityFrames,
											 TArray<ULinkFrames*>& OutLinkFrames,
											 TArray<UMotionFrames*>& OutMotionFrames,
											 TArray<UImpulseFrames*>& OutImpulseFrames,
											 TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames)
{
	for(auto FrameDataEntry : FrameData)
	{
		UHitboxFrames* HitboxFrames = Cast<UHitboxFrames>(FrameDataEntry);
		if(HitboxFrames != nullptr)
		{
			OutHitboxFrames.Add(HitboxFrames);
		}

		UArmorFrames* ArmorFrames = Cast<UArmorFrames>(FrameDataEntry);
		if(ArmorFrames != nullptr)
		{
			OutArmorFrames.Add(ArmorFrames);
		}

		UInvincibilityFrames* InvincibilityFrames = Cast<UInvincibilityFrames>(FrameDataEntry);
		if(InvincibilityFrames != nullptr)
		{
			OutInvincibilityFrames.Add(InvincibilityFrames);
		}

		ULinkFrames* LinkFrames = Cast<ULinkFrames>(FrameDataEntry);
		if(LinkFrames != nullptr)
		{
			OutLinkFrames.Add(LinkFrames);
		}

		UMotionFrames* MotionFrames = Cast<UMotionFrames>(FrameDataEntry);
		if(MotionFrames != nullptr)
		{
			OutMotionFrames.Add(MotionFrames);
		}

		UImpulseFrames* ImpulseFrames = Cast<UImpulseFrames>(FrameDataEntry);
		if(ImpulseFrames != nullptr)
		{
			OutImpulseFrames.Add(ImpulseFrames);
		}

		UMovementPropertyFrames* MovementPropertyFrames = Cast<UMovementPropertyFrames>(FrameDataEntry);
		if(MovementPropertyFrames != nullptr)
		{
			OutMovementPropertyFrames.Add(MovementPropertyFrames);
		}
	}
}

void UMovesetComponent::HandlePlaybackFinished()
{
	if(CurrentMove != nullptr)
	{
		EvaluateNode(0, CurrentMove->GetFinishNode());
	}
}
