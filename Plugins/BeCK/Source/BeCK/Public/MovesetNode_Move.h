#pragma once

#include "MovesetNode_Base.h"
#include "MovesetNode_Move.generated.h"

class UMove;

UCLASS(BlueprintType)
class BECK_API UMovesetNode_Move : public UMovesetNode_Base
{
	GENERATED_BODY()

public:
	virtual UMovesetNode_Base* GetUpdateNode() const { return UpdateNode; };
	virtual UMovesetNode_Base* GetFinishNode() const { return FinishNode; };
	virtual TArray<UMovesetNode_Base*> GetLinkNodes() const { return LinkNodes; };

#if WITH_EDITOR
	virtual void SetUpdateNode(UMovesetNode_Base* InUpdateNode);
	virtual void SetFinishNode(UMovesetNode_Base* InFinishNode);
	virtual void SetLinkNodes(TArray<UMovesetNode_Base*> InLinkNodes);
#endif

	// UMovesetNode_Base interface
#if WITH_EDITOR
	virtual FText GetTitle() const override;
#endif
	virtual int32 GetMinChildNodes() const override { return 0; };
	virtual int32 GetMaxChildNodes() const override { return MAX_ALLOWED_CHILD_NODES; };

public:
	UPROPERTY(EditAnywhere, Instanced, Category = MoveNode)
	UMove* Move;

	UPROPERTY()
	UMovesetNode_Base* UpdateNode;

	UPROPERTY()
	UMovesetNode_Base* FinishNode;

	UPROPERTY()
	TArray<UMovesetNode_Base*> LinkNodes;
};

