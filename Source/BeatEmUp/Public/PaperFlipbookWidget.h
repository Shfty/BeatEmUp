// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Attribute.h"
#include "Styling/SlateBrush.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Components/Widget.h"
#include "PaperFlipbookWidget.generated.h"

class SPaperFlipbook;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class USlateBrushAsset;
class UTexture2D;

DECLARE_DYNAMIC_DELEGATE_RetVal(UPaperFlipbook*, FGetPaperFlipbook);

UCLASS()
class BEATEMUP_API UPaperFlipbookWidget : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flipbook)
	UPaperFlipbook* Flipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flipbook)
	bool Playing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flipbook)
	bool Looping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flipbook)
	bool ReversePlayback;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flipbook)
	int StartingFrame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Flipbook)
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (sRGB = "true"))
	FLinearColor ColorAndOpacity;

	UPROPERTY()
	FGetLinearColor ColorAndOpacityDelegate;

public:
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnPointerEvent OnMouseButtonDownEvent;

public:
	UFUNCTION(BlueprintCallable, Category = "Flipbook")
	void SetFlipbook(UPaperFlipbook* InFlipbook);

	UFUNCTION(BlueprintCallable, Category = "Flipbook")
	void SetPlaying(bool InPlaying);

	UFUNCTION(BlueprintCallable, Category = "Flipbook")
	void SetLooping(bool InLooping);

	UFUNCTION(BlueprintCallable, Category = "Flipbook")
	void SetReversePlayback(bool InReversePlayback);

	UFUNCTION(BlueprintCallable, Category = "Flipbook", meta = (ClampMin = "0"))
	void SetStartingFrame(int InStartingFrame);

	UFUNCTION(BlueprintCallable, Category = "Flipbook")
	void SetPlayRate(float InPlayRate);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetOpacity(float InOpacity);

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	//~ Begin UWidget Interface
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);

protected:
	TSharedPtr<SPaperFlipbook> MyFlipbook;
};
