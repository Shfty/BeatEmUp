// Fill out your copyright notice in the Description page of Project Settings.

#include "BeCK.h"
#include "BeCKMovementComponent.h"

UBeCKMovementComponent::UBeCKMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	ComponentRadius(8.0f),
	ComponentHalfHeight(16.0f),
	GravityScale(1.0f),
	Acceleration(4096.0f),
	Deceleration(2048.0f),
	Mass(65.0f),
	Friction(0.2f),
	bIsSliding(false),
	PreviousHitTime(1.0f),
	PreviousHitNormal(FVector::UpVector)
{
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;
	bUseAccelerationForPaths = true;
	Velocity = FVector::ZeroVector;

	NavAgentProps.bCanWalk = true;
	NavAgentProps.bCanJump = true;
}

void UBeCKMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if(Velocity.SizeSquared() > 0.f)
	{
		UpdateComponentVelocity();
	}
}

void UBeCKMovementComponent::TickMovement(float DeltaTime)
{
	FVector InputVector = ConsumeInputVector();
	InputVector.Z = 0.0f;
	InputVector = InputVector.GetClampedToMaxSize(1.0f);

	if(ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	if(!IsValid(UpdatedComponent))
	{
		return;
	}

	AActor* ActorOwner = UpdatedComponent->GetOwner();
	if(!ActorOwner)
	{
		return;
	}

	if(UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	if(InputVector != FVector::ZeroVector && Acceleration > 0.0f)
	{
		AddForce(InputVector * Acceleration, true);
	}
	else
	{
		FVector TempVelocity = Velocity;
		TempVelocity.Z = 0;
		float VelocityMag = TempVelocity.Size();

		if(VelocityMag > 0.0f)
		{
			if(Velocity.Size() < Deceleration * DeltaTime)
			{
				Velocity = FVector::ZeroVector;
			}
			else
			{
				FVector DecelerationVector = Velocity;
				DecelerationVector.Normalize();
				DecelerationVector *= -Deceleration;
				AddForce(DecelerationVector, true);
			}
		}
	}

	ApplyAccumulatedForces(DeltaTime);

	FHitResult Hit(1.f);
	const FVector OldVelocity = Velocity;
	const FVector MoveDelta = ComputeMoveDelta(OldVelocity, DeltaTime);

	TGuardValue<EMoveComponentFlags> ScopedFlagRestore(MoveComponentFlags, MoveComponentFlags | MOVECOMP_NeverIgnoreBlockingOverlaps);
	MoveUpdatedComponent(MoveDelta, UpdatedComponent->GetComponentQuat(), true, &Hit);

	// If we hit a trigger that destroyed us, abort.
	if(ActorOwner->IsPendingKill())
	{
		return;
	}

	// Handle hit result after movement
	if(!Hit.bBlockingHit)
	{
		PreviousHitTime = 1.f;
		bIsSliding = false;

		// Only calculate new velocity if events didn't change it during the movement update.
		if(Velocity == OldVelocity)
		{
			Velocity = ComputeVelocity(Velocity, DeltaTime);
		}
	}
	else
	{
		// Only calculate new velocity if events didn't change it during the movement update.
		if(Velocity == OldVelocity)
		{
			// re-calculate end velocity for partial time
			Velocity = (Hit.Time > KINDA_SMALL_NUMBER) ? ComputeVelocity(OldVelocity, DeltaTime * Hit.Time) : OldVelocity;
		}

		float SubTickTimeRemaining = DeltaTime * (1.f - Hit.Time);
		HandleDeflection(Hit, Velocity, SubTickTimeRemaining);
	}

	UpdateGrounded();

	UpdateComponentVelocity();
}

FVector UBeCKMovementComponent::ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const
{
	// Velocity Verlet integration (http://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet)
	// The addition of p0 is done outside this method, we are just computing the delta.
	// p = p0 + v0*t + 1/2*a*t^2

	// We use ComputeVelocity() here to infer the acceleration, to make it easier to apply custom velocities.
	// p = p0 + v0*t + 1/2*((v1-v0)/t)*t^2
	// p = p0 + v0*t + 1/2*((v1-v0))*t

	const FVector NewVelocity = ComputeVelocity(InVelocity, DeltaTime);
	const FVector Delta = (InVelocity * DeltaTime) + (NewVelocity - InVelocity) * (0.5f * DeltaTime);
	return Delta;
}

FVector UBeCKMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	FVector NewAcceleration(FVector::ZeroVector);

	NewAcceleration.Z += GetGravityZ();

	return NewAcceleration;
}

FVector UBeCKMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	// v = v0 + a*t
	const FVector NewAcceleration = ComputeAcceleration(InitialVelocity, DeltaTime);
	FVector NewVelocity = InitialVelocity + (NewAcceleration * DeltaTime);

	return LimitVelocity(NewVelocity);
}

FVector UBeCKMovementComponent::LimitVelocity(FVector NewVelocity) const
{
	const float CurrentMaxSpeed = GetMaxSpeed();
	if(CurrentMaxSpeed >= 0.0f)
	{
		FVector LateralVelocity = NewVelocity;
		LateralVelocity.Z = 0.0f;
		LateralVelocity = LateralVelocity.GetClampedToMaxSize(CurrentMaxSpeed);

		NewVelocity = FVector(LateralVelocity.X, LateralVelocity.Y, NewVelocity.Z);
	}

	return ConstrainDirectionToPlane(NewVelocity);
}

bool UBeCKMovementComponent::HandleDeflection(FHitResult& Hit, const FVector& OldVelocity, float& SubTickTimeRemaining)
{
	const FVector Normal = ConstrainNormalToPlane(Hit.Normal);

	// Zero Z velocity if we just hit the floor
	const float LandDotTolerance = 0.5f;
	if(Normal.Z >= LandDotTolerance)
	{
		Velocity.Z = 0.0f;
	}

	// if velocity still into wall (after HandleBlockingHit() had a chance to adjust), slide along wall
	const float SlideDotTolerance = 0.01f;
	bIsSliding = FVector::Coincident(PreviousHitNormal, Normal) || (Velocity.GetSafeNormal() | Normal) <= SlideDotTolerance;

	if(bIsSliding)
	{
		//adjust to move along new wall
		Velocity = ComputeSlideVector(Velocity, 1.f, Normal, Hit);

		// Velocity is now parallel to the impact surface.
		if(SubTickTimeRemaining > KINDA_SMALL_NUMBER)
		{
			if(!HandleSliding(Hit, SubTickTimeRemaining))
			{
				return false;
			}
		}
	}

	return true;
}

bool UBeCKMovementComponent::HandleSliding(FHitResult& Hit, float& SubTickTimeRemaining)
{
	FHitResult InitialHit(Hit);
	const FVector OldHitNormal = ConstrainDirectionToPlane(Hit.Normal);

	// Velocity is now parallel to the impact surface.
	// Perform the move now, before adding gravity/accel again, so we don't just keep hitting the surface.
	SafeMoveUpdatedComponent(Velocity * SubTickTimeRemaining, UpdatedComponent->GetComponentQuat(), true, Hit);

	// Find velocity after elapsed time
	const FVector PostTickVelocity = ComputeVelocity(Velocity, SubTickTimeRemaining);

	// If pointing back into surface, apply friction and acceleration.
	const FVector Force = (PostTickVelocity - Velocity);
	const float ForceDotN = (Force | OldHitNormal);
	if(ForceDotN < 0.f)
	{
		const FVector ProjectedForce = FVector::VectorPlaneProject(Force, OldHitNormal);
		const FVector NewVelocity = Velocity + ProjectedForce;

		const FVector FrictionForce = -NewVelocity.GetSafeNormal() * FMath::Min(-ForceDotN * Friction, NewVelocity.Size());
		Velocity = ConstrainDirectionToPlane(NewVelocity + FrictionForce);
	}
	else
	{
		Velocity = PostTickVelocity;
	}

	SubTickTimeRemaining = 0.f;

	return true;
}

void UBeCKMovementComponent::UpdateGrounded()
{
	FHitResult GroundTraceResult = FHitResult();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("BeCKMovement Ground Check")), true, this->GetOwner());
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	bIsGrounded = GetWorld()->SweepSingleByObjectType(
		GroundTraceResult,
		UpdatedComponent->ComponentToWorld.GetLocation(),
		UpdatedComponent->ComponentToWorld.GetLocation() - FVector(0.0f, 0.0f, (ComponentHalfHeight - ComponentRadius) + 0.1f),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(ComponentRadius),
		TraceParams
	);
}

bool UBeCKMovementComponent::IsMovingOnGround() const
{
	return bIsGrounded;
}

float UBeCKMovementComponent::GetGravityZ() const
{
	return ShouldApplyGravity() ? Super::GetGravityZ() * GravityScale : 0.f;
}

void UBeCKMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	FVector Move = FVector(MoveVelocity);
	Move.Z = 0.0f;
	Move = Move.GetClampedToMaxSize(1.0f);
	PawnOwner->Internal_AddMovementInput(Move);
}

void UBeCKMovementComponent::RequestPathMove(const FVector& MoveInput)
{
	FVector Move = MoveInput;
	Move.Z = 0.0f;
	PawnOwner->Internal_AddMovementInput(Move);
}

void UBeCKMovementComponent::AddImpulse(FVector Impulse, bool bVelocityChange)
{
	if(!Impulse.IsZero())
	{
		// handle scaling by mass
		FVector FinalImpulse = Impulse;
		if(!bVelocityChange)
		{
			if(Mass > SMALL_NUMBER)
			{
				FinalImpulse = FinalImpulse / Mass;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Attempt to apply impulse to zero or negative Mass in BeCKMovement"));
			}
		}

		PendingImpulseToApply += FinalImpulse;
	}
}

void UBeCKMovementComponent::AddForce(FVector Force, bool bAccelChange)
{
	if(!Force.IsZero())
	{
		// handle scaling by mass
		FVector FinalForce = Force;
		if(!bAccelChange)
		{
			if(Mass > SMALL_NUMBER)
			{
				FinalForce = FinalForce / Mass;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Attempt to apply force to zero or negative Mass in BeCKMovement"));
			}
		}

		PendingForceToApply += FinalForce;
	}
}

void UBeCKMovementComponent::ApplyAccumulatedForces(float DeltaSeconds)
{
	Velocity += PendingImpulseToApply + (PendingForceToApply * DeltaSeconds);

	PendingImpulseToApply = FVector::ZeroVector;
	PendingForceToApply = FVector::ZeroVector;
}
