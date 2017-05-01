// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/NavMovementComponent.h"
#include "BeatEmUpMovementComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), ShowCategories = (Velocity))
class BEATEMUP_API UBeatEmUpMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	UBeatEmUpMovementComponent(const FObjectInitializer& ObjectInitializer);

public:
	void UBeatEmUpMovementComponent::InitializeComponent() override;

	virtual float GetMaxSpeed() const override { return MaxSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Beat Em Up Movement Component")
	void TickMovement(float DeltaTime);

	FVector ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const;
	FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const;
	FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const;
	FVector LimitVelocity(FVector NewVelocity) const;
	bool HandleDeflection(FHitResult& Hit, const FVector& OldVelocity, float& SubTickTimeRemaining);
	bool HandleSliding(FHitResult& Hit, float& SubTickTimeRemaining);

	void UpdateGrounded();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|BeatEmUpMovement")
	void AddImpulse(FVector Impulse, bool bVelocityChange);

	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|BeatEmUpMovement")
	void AddForce(FVector Force, bool bAccelChange);

	void ApplyAccumulatedForces(float DeltaSeconds);

	bool ShouldApplyGravity() const { return GravityScale != 0.f; }

	virtual bool IsMovingOnGround() const override;

	/** Compute gravity effect given current physics volume, projectile gravity scale, etc. */
	virtual float GetGravityZ() const override;

	//BEGIN UNavMovementComponent Interface
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;
	virtual void RequestPathMove(const FVector& MoveInput) override;

public:

	/** Limit on speed (< 0 means no limit). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement)
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement)
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement)
	float Mass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement)
	float ComponentRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement)
	float ComponentHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement)
	float GravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BeatEmUpMovement, meta = (ClampMin = "0", UIMin = "0"))
	float Friction;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "BeatEmUpMovement|State")
	uint32 bIsGrounded : 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "BeatEmUpMovement|State")
	uint32 bIsSliding : 1;

	/** Saved HitResult Time (0 to 1) from previous simulation step. Equal to 1.0 when there was no impact. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "BeatEmUpMovement|State")
	float PreviousHitTime;

	/** Saved HitResult Normal from previous simulation step that resulted in an impact. If PreviousHitTime is 1.0, then the hit was not in the last step. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "BeatEmUpMovement|State")
	FVector PreviousHitNormal;

	/** Accumulated impulse to be added next tick. */
	UPROPERTY(Transient)
	FVector PendingImpulseToApply;

	/** Accumulated force to be added next tick. */
	UPROPERTY(Transient)
	FVector PendingForceToApply;
};
