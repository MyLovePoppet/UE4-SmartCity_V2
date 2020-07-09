// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "CustomGravityPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class SMARTCITYOS_API UCustomGravityPawnMovement : public UFloatingPawnMovement
{
    GENERATED_BODY()
public:
    UCustomGravityPawnMovement();

    //Begin UActorComponent Interface
    //virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
    //End UActorComponent Interface

    //BEGIN UMovementComponent Interface
    virtual bool IsMovingOnGround() const override;
    virtual bool IsFalling() const override;
    //END UMovementComponent Interface

    virtual void InitializeComponent() override;

    virtual void DoJump();

    virtual void DoSprint();

    virtual void DoStopSprint();

    void CapsuleHited(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
                      FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);


    /** Update the custom pawn that owns this component. */
    void SetComponentOwner(APawn* Owner);

    virtual void UpdateCapsuleRotation(float DeltaTime, const FVector& TargetUpVector);

    FVector GetGravityDirection() const;

    void SetEarthLocation(FVector _EarthLocation) { EarthLocation = _EarthLocation; }

protected:
    /** Update Velocity based on input. Also applies gravity. */
    virtual void ApplyControlInputToVelocity(float DeltaTime) override;

    /**The Updated component*/
    UCapsuleComponent* CapsuleComponent;

    /** Custom movement component owner */
    APawn* PawnOwner;

public:
    //是否启用GravityMovement
    bool bIsWork = false;
    float GravityPower = 980.f;
    FVector EarthLocation;

    /** If true, Pawn can jump. */
    bool bCanJump;

    /** Desired jump height */
    float JumpHeight = 300.f;

    /** Maximum acceptable distance for custom pawn capsule/sphere to walk above a surface. */
    //	float GroundHitToleranceDistance;

    /** When sprinting, multiplier applied to Max Walk Speed */
    float SpeedBoostMultiplier;

    /**
    * When falling, amount of lateral movement control available to the character.
    * 0 = no control, 1 = full control at max speed of MaxWalkSpeed.
    */
    float AirControlRatio;

private:

    bool bIsHit;

    bool bIsInAir;

    FRotator CurrentCapsuleRotation;

    float TimeInAir;

    float LastWalkSpeed;

    bool bIsSprinting;

    bool bIsJumping;
};
