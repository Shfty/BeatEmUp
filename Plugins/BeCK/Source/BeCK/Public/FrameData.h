// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FrameData.generated.h"

UCLASS(Abstract, DefaultToInstanced, EditInlineNew, BlueprintType)
class BECK_API UFrameData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FrameData)
	FInt32Interval ActivePeriod;

	UFrameData() :
		ActivePeriod(FInt32Interval(0, 0))
	{

	}
};

UCLASS()
class BECK_API UHitboxFrames : public UFrameData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hitbox)
	int32 HitIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hitbox)
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hitbox)
	FVector2D Location;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hitbox)
	FVector2D Size;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hitbox)
	FVector2D Impulse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hitbox)
	uint32 VelocityChange : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hitbox)
	uint32 VelocityReset : 1;

	UHitboxFrames() :
		HitIndex(0),
		Damage(1),
		Location(FVector2D(0.0f, -10.0f)),
		Size(FVector2D(10.0f, 10.0f)),
		Impulse(FVector2D::ZeroVector),
		VelocityChange(false),
		VelocityReset(false)
	{

	}
};

UCLASS()
class BECK_API UArmorFrames : public UFrameData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Armor)
	float DamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Armor)
	uint32 HasHitLimit : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Armor, meta = (EditCondition = "HasHitLimit"))
	int32 HitLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Armor)
	uint32 HasDamageLimit : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Armor, meta = (EditCondition = "HasDamageLimit"))
	int32 DamageLimit;

	UArmorFrames() :
		DamageMultiplier(0.0f),
		HasHitLimit(false),
		HitLimit(0),
		HasDamageLimit(false),
		DamageLimit(0)
	{

	}
};

UCLASS()
class BECK_API UInvincibilityFrames : public UFrameData
{
	GENERATED_BODY()

};

UCLASS()
class BECK_API ULinkFrames : public UFrameData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Link)
	int32 LinkIndex;
};

class UCurveVector;

UCLASS()
class BECK_API UMotionFrames : public UFrameData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	UCurveVector* MotionCurve;
};

UCLASS()
class BECK_API UImpulseFrames : public UFrameData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Impulse)
	FVector2D Impulse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Impulse)
	uint32 VelocityChange : 1;

	UImpulseFrames() :
		Impulse(FVector2D::ZeroVector),
		VelocityChange(false)
	{

	}
};

UCLASS()
class BECK_API UMovementPropertyFrames : public UFrameData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
	float MaxSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
	float Acceleration;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Property)
	float GravityScale;

	UMovementPropertyFrames() :
		MaxSpeed(-1.0f),
		Acceleration(0.0f),
		Deceleration(0.0f),
		GravityScale(1.0f)
	{

	}
};