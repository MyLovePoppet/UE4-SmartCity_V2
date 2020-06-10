// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGravityPawnMovement.h"

#include "Components/CapsuleComponent.h"

UCustomGravityPawnMovement::UCustomGravityPawnMovement(): Super()
{
    // Initialization

    //Custom Movement Component

    bIsInAir = false;
    GravityScale = 2.0f;
    bCanJump = true;
    JumpHeight = 100.0f;
    ////	GroundHitToleranceDistance = 20.0f;
    SpeedBoostMultiplier = 2.0f;
    AirControlRatio = 0.5f;

    //PlanetActor = nullptr;

    // Floating Pawn Movement

    MaxSpeed = 500.0;
    Acceleration = 2048.0f;
    Deceleration = 2048;
}

void UCustomGravityPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateCapsuleRotation(DeltaTime, -GetGravityDirection());
}

bool UCustomGravityPawnMovement::IsMovingOnGround() const
{
    return !bIsInAir;
}

bool UCustomGravityPawnMovement::IsFalling() const
{
    return bIsInAir;
}

// Initializes the component
void UCustomGravityPawnMovement::InitializeComponent()
{
    Super::InitializeComponent();

    if (UpdatedComponent == NULL) { return; }
    CapsuleComponent = Cast<UCapsuleComponent>(UpdatedComponent);

    UpdateNavAgent(*CapsuleComponent);
}

void UCustomGravityPawnMovement::DoJump()
{
    if (bIsInAir) { return; }
    //bIsInAir = true;
    const float TargetJumpHeight = JumpHeight + CapsuleComponent->GetScaledCapsuleHalfHeight();
    //const FVector JumpImpulse = CapsuleComponent->GetUpVector() * FMath::Sqrt(TargetJumpHeight * 2.f * GravityPower);
    //const bool bUseAccl = true;
    const FVector AddUpVelocity = -GetGravityDirection() * FMath::Sqrt(2 * GravityPower * TargetJumpHeight);
    Velocity += AddUpVelocity;
    //CapsuleComponent->GetBodyInstance()->AddImpulse(JumpImpulse, bUseAccl);
}

void UCustomGravityPawnMovement::DoSprint()
{
    if (bIsInAir || bIsSprinting) { return; }

    LastWalkSpeed = MaxSpeed;
    MaxSpeed *= SpeedBoostMultiplier;
    bIsSprinting = true;
}

void UCustomGravityPawnMovement::DoStopSprint()
{
    GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, "Stop");
    MaxSpeed = LastWalkSpeed;
    bIsSprinting = false;
}

void UCustomGravityPawnMovement::CapsuleHited(class UPrimitiveComponent* MyComp, class AActor* Other,
                                              class UPrimitiveComponent* OtherComp, bool bSelfMoved,
                                              FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                                              const FHitResult& Hit)
{
    SinAngleHitPlane = FMath::Sqrt(
        FVector::VectorPlaneProject((-GetGravityDirection()), HitNormal).SizeSquared() / GetGravityDirection().
        SizeSquared());

    Velocity = FVector::VectorPlaneProject(Velocity, HitNormal);
}


void UCustomGravityPawnMovement::ApplyControlInputToVelocity(float DeltaTime)
{
    const FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);

    const float AnalogInputModifier = (ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f);
    const float MaxPawnSpeed = GetMaxSpeed() * AnalogInputModifier;
    const bool bExceedingMaxSpeed = IsExceedingMaxSpeed(MaxPawnSpeed);

    if (AnalogInputModifier <= 0.f)
    {
        // Dampen velocity magnitude based on deceleration.
        if (Velocity.SizeSquared() > 0.f)
        {
            const FVector OldVelocity = Velocity;
            if (!bIsInAir)
            {
                const float DecelerationSpeed = FMath::Abs(Deceleration) * DeltaTime;
                if (Velocity.SizeSquared() <= FMath::Square(DecelerationSpeed))
                {
                    Velocity = Velocity.ProjectOnToNormal(GetGravityDirection());
                }
                else
                {
                    Velocity -= FVector::VectorPlaneProject(Velocity.GetSafeNormal(), GetGravityDirection()) *
                        DecelerationSpeed;
                }
            }

            // Don't allow braking to lower us below max speed if we started above it.
            if (bExceedingMaxSpeed && Velocity.SizeSquared() < FMath::Square(MaxPawnSpeed))
            {
                Velocity = OldVelocity.GetSafeNormal() * MaxPawnSpeed;
            }
        }
    }

    FVector Gravity = GetGravityDirection() * GravityPower; //GetGravityZ();
    Velocity += Gravity * DeltaTime;

    // Apply acceleration and clamp velocity magnitude.
    const float NewMaxSpeed = (IsExceedingMaxSpeed(MaxPawnSpeed)) ? Velocity.Size() : MaxPawnSpeed;
    Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime;
    Velocity = Velocity.GetClampedToMaxSize(NewMaxSpeed);

    ConsumeInputVector();
}

void UCustomGravityPawnMovement::SetComponentOwner(APawn* Owner)
{
    PawnOwner = Owner;
}

void UCustomGravityPawnMovement::UpdateCapsuleRotation(float DeltaTime, const FVector& TargetUpVector)
{
    const FVector CapsuleUp = CapsuleComponent->GetUpVector();
    const FQuat DeltaQuat = FQuat::FindBetween(CapsuleUp, TargetUpVector);
    const FQuat TargetQuat = DeltaQuat * CapsuleComponent->GetComponentRotation().Quaternion();

    CapsuleComponent->SetWorldRotation(TargetQuat);
}

FVector UCustomGravityPawnMovement::GetGravityDirection() const
{
    if (!PlanetActor)
    {
        return FVector(0.f, 0.f, -1.f);
    }
    return (PlanetActor->GetActorLocation() - CapsuleComponent->GetComponentLocation()).GetSafeNormal();
}
