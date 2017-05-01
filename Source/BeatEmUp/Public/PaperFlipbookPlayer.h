// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PaperFlipbookPlayer.generated.h"

class UPaperFlipbook;

DECLARE_DELEGATE_OneParam(FFlipbookChangedFrameDelegate, int32)
DECLARE_DELEGATE_OneParam(FFlipbookChangedSpriteDelegate, int32)
DECLARE_DELEGATE(FFlipbookFinishedDelegate)

UCLASS(hidecategories = object, MinimalAPI, BlueprintType)
class UPaperFlipbookPlayer : public UObject
{
	GENERATED_BODY()

public:
	UPaperFlipbookPlayer();

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	UPaperFlipbook* GetFlipbook() const { return PaperFlipbook; };

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	bool GetPlaying() const { return Playing; };

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	bool GetLooping() const { return Looping; };

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	bool GetReversePlayback() const { return ReversePlayback; };

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	float GetPlayRate() const { return PlayRate; };

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void Play();

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void PlayFromStart();

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void Reverse();

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void ReverseFromEnd();

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void Stop();

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	float GetPlaybackPosition() const;

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	int32 GetTimeInFrames(float Time) const;

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	int32 GetPlaybackPositionInFrames() const;

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	float GetFlipbookLength() const;

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	int32 GetFlipbookLengthInFrames() const;

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	float GetFlipbookFramerate() const;

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void SetFlipbook(UPaperFlipbook* NewFlipbook);

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void SetPlaybackPosition(float NewPosition, bool bFireEvents);

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void SetPlaybackPositionInFrames(int32 NewFramePosition, bool bFireEvents);

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void SetNewTime(float NewTime);

	UFUNCTION(BlueprintCallable, Category = "Flipbook Player")
	void TickFlipbook(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Playing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Looping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ReversePlayback;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayRate;

	FOnFloatValueChanged OnPlaybackPositionChanged;
	FFlipbookChangedFrameDelegate OnFrameChanged;
	FFlipbookChangedSpriteDelegate OnSpriteChanged;
	FFlipbookFinishedDelegate OnPlaybackFinished;

protected:
	UPROPERTY()
	float AccumulatedTime;

private:
	UPaperFlipbook* PaperFlipbook;

	int32 CachedFrameIndex;
	int32 CachedKeyframeIndex;
};
