// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MovesetComponent.generated.h"

class UMoveset;
class UPaperFlipbookPlayer;
class UMovesetNode_Base;
class UMovesetNode_Move;
class UFrameData;
class UHitboxFrames;
class UArmorFrames;
class UInvincibilityFrames;
class ULinkFrames;
class UMotionFrames;
class UImpulseFrames;
class UActorChannel;
class FOutBunch;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveChanged, UMovesetNode_Move*, NewMove);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaybackPositionChanged, float, NewPlaybackPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFrameChanged, int32, NewFrame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpriteChanged, UPaperSprite*, NewSprite);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMovesetEvent, FName, EventName, FVector, EventLocation, FVector, EventDirection);

USTRUCT()
struct FMovesetInputState
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<bool> CurrentState;

	UPROPERTY()
	TArray<bool> PrevState;
};

UCLASS(ClassGroup=(Moveset), meta=(BlueprintSpawnableComponent))
class BEATEMUP_API UMovesetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovesetComponent(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	UMovesetNode_Move* GetCurrentMove() const;

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	void SetCurrentMove(UMovesetNode_Move* NewMove, bool FireEvents = true, bool bForce = false);

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	bool GetInput(uint8 Input) const;

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	void SetInput(uint8 Input, bool Value);

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	void GetInputs(TArray<bool>& Inputs, TArray<bool>& PrevInputs) const { Inputs = InputState.CurrentState; PrevInputs = InputState.PrevState; };

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	void SetInputs(TArray<bool> NewInputs, TArray<bool> NewPrevInputs);

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	void TickMoveset(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	void PostTickMoveset();

	UFUNCTION(BlueprintPure, Category = "Moveset Component")
	virtual void GetFrameData(TArray<UHitboxFrames*>& OutHitboxFrames,
							  TArray<UArmorFrames*>& OutArmorFrames,
							  TArray<UInvincibilityFrames*>& OutInvincibilityFrames,
							  TArray<ULinkFrames*>& OutLinkFrames,
							  TArray<UMotionFrames*>& OutMotionFrames,
							  TArray<UImpulseFrames*>& OutImpulseFrames,
							  TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames);

	UFUNCTION(BlueprintPure, Category = "Moveset Component")
	virtual void GetFrameDataForFrame(int32 Frame,
									  TArray<UHitboxFrames*>& OutHitboxFrames,
									  TArray<UArmorFrames*>& OutArmorFrames,
									  TArray<UInvincibilityFrames*>& OutInvincibilityFrames,
									  TArray<ULinkFrames*>& OutLinkFrames,
									  TArray<UMotionFrames*>& OutMotionFrames,
									  TArray<UImpulseFrames*>& OutImpulseFrames,
									  TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames);

	UFUNCTION(BlueprintPure, Category = "Moveset Component")
	virtual void GetFrameDataForCurrentFrame(TArray<UHitboxFrames*>& OutHitboxFrames,
											 TArray<UArmorFrames*>& OutArmorFrames,
											 TArray<UInvincibilityFrames*>& OutInvincibilityFrames,
											 TArray<ULinkFrames*>& OutLinkFrames,
											 TArray<UMotionFrames*>& OutMotionFrames,
											 TArray<UImpulseFrames*>& OutImpulseFrames,
											 TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames);

	UFUNCTION(BlueprintCallable, Category = "Moveset Component")
	virtual void SortFrameDataByClass(TArray<UFrameData*> FrameData,
									  TArray<UHitboxFrames*>& OutHitboxFrames,
									  TArray<UArmorFrames*>& OutArmorFrames,
									  TArray<UInvincibilityFrames*>& OutInvincibilityFrames,
									  TArray<ULinkFrames*>& OutLinkFrames,
									  TArray<UMotionFrames*>& OutMotionFrames,
									  TArray<UImpulseFrames*>& OutImpulseFrames,
									  TArray<UMovementPropertyFrames*>& OutMovementPropertyFrames);

	// UActorComponent Interface
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	bool EvaluateNode(int TreeDepth, UMovesetNode_Base* Node);
	void HandlePlaybackPositionChanged(float NewPlaybackPosition);
	void HandleSpriteChanged(int32 NewSpriteIndex);
	void HandleFrameChanged(int32 NewFrame);
	void HandlePlaybackFinished();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMoveset* Moveset;

	UPROPERTY(BlueprintReadOnly)
	UPaperFlipbookPlayer* FlipbookPlayer;

	UPROPERTY(BlueprintAssignable)
	FOnMoveChanged OnMoveChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlaybackPositionChanged OnPlaybackPositionChanged;

	UPROPERTY(BlueprintAssignable)
	FOnFrameChanged OnFrameChanged;

	UPROPERTY(BlueprintAssignable)
	FOnSpriteChanged OnSpriteChanged;

	UPROPERTY(BlueprintAssignable)
	FOnMovesetEvent OnMovesetEvent;

protected:
	UPROPERTY(Transient)
	UMovesetNode_Move* CurrentMove;

	UPROPERTY(Transient)
	FMovesetInputState InputState;

	UPROPERTY(Transient)
	TArray<bool> PressedState;

	UPROPERTY(Transient)
	TArray<bool> ReleasedState;
};
