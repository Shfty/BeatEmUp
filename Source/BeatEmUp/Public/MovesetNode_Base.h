// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "MovesetNode_Base.generated.h"

class UEdGraphNode;
class UMovesetNode_Base;
struct FPropertyChangedEvent;

UCLASS(ABSTRACT, hidecategories = Object, EditInlineNew)
class BEATEMUP_API UMovesetNode_Base : public UObject
{
	GENERATED_BODY()

public:
	UMovesetNode_Base(const FObjectInitializer& ObjectInitializer);

public:
	virtual UMovesetNode_Base* GetExecNode() const
	{
		return ExecNode;
	};

	virtual void GetChildNodes(TArray<UMovesetNode_Base*>& MoveNodes);

	virtual int32 GetMinChildNodes() const
	{
		return 0;
	}

	virtual int32 GetMaxChildNodes() const
	{
		return 0;
	}

	// UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
#endif
	virtual void Serialize(FArchive& Ar) override;


#if WITH_EDITOR
	// Editor interface.
	virtual void CreateStartingConnectors(void);
	virtual void InsertChildNode(int32 Index);
	virtual void RemoveChildNode(int32 Index);

	virtual void SetChildNodes(TArray<UMovesetNode_Base*>& InChildNodes);
	virtual void SetExecNode(UMovesetNode_Base* InExecNode);

	virtual FText GetInputPinName(int32 PinIndex) const { return FText::GetEmpty(); }
	virtual FText GetTitle() const;

	void PlaceNode(int32 NodeColumn, int32 NodeRow, int32 RowCount);

	virtual void OnBeginPIE(const bool bIsSimulating) {};
	virtual void OnEndPIE(const bool bIsSimulating) {};
#endif //WITH_EDITOR

public:
	static const int32 MAX_ALLOWED_CHILD_NODES = 32;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName NodeName;

	UPROPERTY()
	TArray<UMovesetNode_Base*> ChildNodes;

	UPROPERTY()
	UMovesetNode_Base* ExecNode;

#if WITH_EDITORONLY_DATA
	/** Node's Graph representation, used to get position. */
	UPROPERTY()
	UEdGraphNode* GraphNode;

	class UEdGraphNode* GetGraphNode() const;
#endif
};

