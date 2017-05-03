#include "BeatEmUp.h"

#include "PaperFlipbookPlayer.h"
#include "PaperFlipbook.h"
#include "Net/UnrealNetwork.h"

UPaperFlipbookPlayer::UPaperFlipbookPlayer() :
	Playing(false),
	Looping(false) ,
	ReversePlayback(false),
	PlayRate(1.0f),
	AccumulatedTime(0.0f),
	CachedKeyframeIndex(INDEX_NONE)
{

}

void UPaperFlipbookPlayer::Play()
{
	Playing = true;
}

void UPaperFlipbookPlayer::Stop()
{
	Playing = false;
}

void UPaperFlipbookPlayer::ReverseFromEnd()
{
	if(PaperFlipbook)
	{
		SetPlaybackPosition(PaperFlipbook->GetTotalDuration(), /*bFireEvents=*/ false);
		Reverse();
	}
}

void UPaperFlipbookPlayer::Reverse()
{
	ReversePlayback = true;
	Playing = true;
}

void UPaperFlipbookPlayer::PlayFromStart()
{
	SetPlaybackPosition(0.0f, /*bFireEvents=*/ false);
	ReversePlayback = false;
	Playing = true;
}

float UPaperFlipbookPlayer::GetPlaybackPosition() const
{
	return AccumulatedTime;
}

int32 UPaperFlipbookPlayer::GetTimeInFrames(float Time) const
{
	const float Framerate = GetFlipbookFramerate();
	const int32 NumFrames = GetFlipbookLengthInFrames();
	if(NumFrames > 0)
	{
		return FMath::Clamp<int32>(FMath::TruncToInt(Time * Framerate), 0, NumFrames - 1);
	}
	else
	{
		return 0;
	}
}

int32 UPaperFlipbookPlayer::GetPlaybackPositionInFrames() const
{
	return GetTimeInFrames(AccumulatedTime);
}

float UPaperFlipbookPlayer::GetFlipbookLength() const
{
	if(PaperFlipbook != nullptr)
	{
		return PaperFlipbook->GetTotalDuration();
	}

	return 0.0f;
}

int32 UPaperFlipbookPlayer::GetFlipbookLengthInFrames() const
{
	if(PaperFlipbook != nullptr)
	{
		return PaperFlipbook->GetNumFrames();
	}

	return 0;
}

float UPaperFlipbookPlayer::GetFlipbookFramerate() const
{
	if(PaperFlipbook != nullptr)
	{
		return PaperFlipbook->GetFramesPerSecond();
	}

	return 15.0f;
}

void UPaperFlipbookPlayer::SetFlipbook(UPaperFlipbook* NewFlipbook)
{
	PaperFlipbook = NewFlipbook;
	CachedFrameIndex = INDEX_NONE;
	CachedKeyframeIndex = INDEX_NONE;
}

void UPaperFlipbookPlayer::SetPlaybackPosition(float NewPosition, bool bFireEvents)
{
	UPaperFlipbook* SourceFlipbook = GetFlipbook();
	if(SourceFlipbook)
	{
		float OldPosition = AccumulatedTime;
		AccumulatedTime = NewPosition;

		OnPlaybackPositionChanged.ExecuteIfBound(AccumulatedTime);
	}

	const int32 LastCachedFrame = CachedFrameIndex;
	CachedFrameIndex = (PaperFlipbook != nullptr) ? GetPlaybackPositionInFrames() : INDEX_NONE;

	if(CachedFrameIndex != LastCachedFrame)
	{
		OnFrameChanged.ExecuteIfBound(CachedFrameIndex);
	}

	const int32 LastCachedKeyframe = CachedKeyframeIndex;
	CachedKeyframeIndex = (PaperFlipbook != nullptr) ? PaperFlipbook->GetKeyFrameIndexAtTime(AccumulatedTime) : INDEX_NONE;

	if(CachedKeyframeIndex != LastCachedKeyframe)
	{
		OnSpriteChanged.ExecuteIfBound(CachedKeyframeIndex);
	}
}

void UPaperFlipbookPlayer::SetPlaybackPositionInFrames(int32 NewFramePosition, bool bFireEvents)
{
	if(PaperFlipbook)
	{
		const float Framerate = PaperFlipbook->GetFramesPerSecond();
		const float NewTime = (Framerate > 0.0f) ? (NewFramePosition / Framerate) : 0.0f;
		SetPlaybackPosition(NewTime, bFireEvents);
	}
}

void UPaperFlipbookPlayer::SetNewTime(float NewTime)
{
	SetPlaybackPosition(NewTime, false);
}

void UPaperFlipbookPlayer::TickFlipbook(float DeltaTime)
{
	if(PaperFlipbook != nullptr)
	{
		bool bIsFinished = false;

		if(GetPlaying())
		{
			const float TimelineLength = PaperFlipbook->GetTotalDuration();
			float PlayRateValue = GetPlayRate();
			const float EffectiveDeltaTime = DeltaTime * (GetReversePlayback() ? -PlayRateValue : PlayRateValue);

			float NewPosition = AccumulatedTime + EffectiveDeltaTime;

			if(EffectiveDeltaTime > 0.0f)
			{
				if(NewPosition > TimelineLength)
				{
					if(GetLooping())
					{
						// If looping, play to end, jump to start, and set target to somewhere near the beginning.
						SetPlaybackPosition(TimelineLength, true);
						SetPlaybackPosition(0.0f, false);

						if(TimelineLength > 0.0f)
						{
							while(NewPosition > TimelineLength)
							{
								NewPosition -= TimelineLength;
							}
						}
						else
						{
							NewPosition = 0.0f;
						}
					}
					else
					{
						// If not looping, snap to end and stop playing.
						NewPosition = TimelineLength;
						Stop();
						bIsFinished = true;
					}
				}
			}
			else
			{
				if(NewPosition < 0.0f)
				{
					if(GetLooping())
					{
						// If looping, play to start, jump to end, and set target to somewhere near the end.
						SetPlaybackPosition(0.0f, true);
						SetPlaybackPosition(TimelineLength, false);

						if(TimelineLength > 0.0f)
						{
							while(NewPosition < 0.0f)
							{
								NewPosition += TimelineLength;
							}
						}
						else
						{
							NewPosition = 0.0f;
						}
					}
					else
					{
						// If not looping, snap to start and stop playing.
						NewPosition = 0.0f;
						Stop();
						bIsFinished = true;
					}
				}
			}

			SetPlaybackPosition(NewPosition, true);
		}

		// Notify user that the flipbook finished playing
		if(bIsFinished)
		{
			OnPlaybackFinished.ExecuteIfBound();
		}
	}
}
