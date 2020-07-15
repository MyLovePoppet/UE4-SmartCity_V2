// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGravityPawnMovement.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UCustomGravityPawnMovement::UCustomGravityPawnMovement(): Super()
{
    // Initialization

    //Custom Movement Component

    bIsInAir = false;
    bCanJump = true;
    JumpHeight = 300.0f;
    ////	GroundHitToleranceDistance = 20.0f;
    SpeedBoostMultiplier = 4.0f;
    AirControlRatio = 0.5f;

    // Floating Pawn Movement

    MaxSpeed = 500.0;
    LastWalkSpeed = MaxSpeed;
    Acceleration = 2048.0f;
    Deceleration = 2048;
}

void UCustomGravityPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
    if(bIsWork)
    {
        FVector Gravity = GetGravityDirection() * GravityPower; //GetGravityZ();
        Velocity += Gravity * DeltaTime;

        Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

        UpdateCapsuleRotation(DeltaTime, -GetGravityDirection());

        /* Local Variables */
        const EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
        const ECollisionChannel CollisionChannel = CapsuleComponent->GetCollisionObjectType();
        const FVector TraceStart = CapsuleComponent->GetComponentLocation();
        const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
        float ShapeRadius = CapsuleComponent->GetScaledCapsuleRadius() * 0.99f;
        FVector TraceEnd = TraceStart - CapsuleComponent->GetUpVector() * (CapsuleHalfHeight - ShapeRadius + 30.f + 1.0f);
        FHitResult HitResult;
        TArray<AActor*> ActorsToIgnore;
        UKismetSystemLibrary::SphereTraceSingle(this, TraceStart, TraceEnd, ShapeRadius,
                                                UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true,
                                                ActorsToIgnore, DrawDebugType, HitResult, true);
        bIsInAir = !HitResult.bBlockingHit;
        TimeInAir = bIsInAir ? TimeInAir + DeltaTime : 0.0f;
    }
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
   // GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, "Stop");
    MaxSpeed = LastWalkSpeed;
    bIsSprinting = false;
}

void UCustomGravityPawnMovement::CapsuleHited(class UPrimitiveComponent* MyComp, class AActor* Other,
                                              class UPrimitiveComponent* OtherComp, bool bSelfMoved,
                                              FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                                              const FHitResult& Hit)
{
    
    // SinAngleHitPlane = FMath::Sqrt(
    //     FVector::VectorPlaneProject((-GetGravityDirection()), HitNormal).SizeSquared() / GetGravityDirection().
    //     SizeSquared());

    //碰撞后将垂直碰撞面的速度置零
    //Velocity = FVector::VectorPlaneProject(Velocity, HitNormal);
}


void UCustomGravityPawnMovement::ApplyControlInputToVelocity(float DeltaTime)
{
    const FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);

    const float AnalogInputModifier = (ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f);
    const float MaxPawnSpeed = GetMaxSpeed(); // * AnalogInputModifier;
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
                    Velocity -= FVector::VectorPlaneProject(Velocity, GetGravityDirection()).GetSafeNormal() *
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


    // Apply acceleration and clamp velocity magnitude.
    //const float NewMaxSpeed = (IsExceedingMaxSpeed(MaxPawnSpeed)) ? Velocity.Size() : MaxPawnSpeed;
    Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime;
    FVector TempVelocity = FVector::VectorPlaneProject(Velocity, GetGravityDirection());
    Velocity -= TempVelocity;
    TempVelocity = TempVelocity.GetClampedToMaxSize(MaxPawnSpeed);
    Velocity += TempVelocity;

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

    FVector grav=(EarthLocation - CapsuleComponent->GetComponentLocation()).GetSafeNormal();
	GEngine->AddOnScreenDebugMessage(2, 20.f, FColor::Purple, grav.ToString());
	return grav;
}
