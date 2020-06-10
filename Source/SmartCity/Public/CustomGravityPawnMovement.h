// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "CustomGravityPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class SMARTCITY_API UCustomGravityPawnMovement : public UFloatingPawnMovement
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
    UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CustomMovementComponent")
    void SetComponentOwner(APawn* Owner);

    virtual void UpdateCapsuleRotation(float DeltaTime, const FVector& TargetUpVector);

    FVector GetGravityDirection() const;

    void setPlanetActor(AActor* Planet) { PlanetActor = Planet; }

protected:
    /** Update Velocity based on input. Also applies gravity. */
    virtual void ApplyControlInputToVelocity(float DeltaTime) override;

    /**The Updated component*/
    UCapsuleComponent* CapsuleComponent;

    /** Custom movement component owner */
    APawn* PawnOwner;

public:

    float GravityPower = 980.f;
    UPROPERTY(Category = "Custom Movement Component : Custom Gravity", EditAnywhere, BlueprintReadWrite)
    AActor* PlanetActor;

    /**
    *Custom Gravity Scale.
    *Gravity is multiplied by this amount for the Component Owner (Pawn).
    *In DefaultGravity case : 0 = No Gravity , Other value than 0 = Default Gravity is enabled
    */
    UPROPERTY(Category = "Custom Movement Component : General Settings", EditAnywhere, BlueprintReadWrite)
    float GravityScale;

    /** If true, Pawn can jump. */
    UPROPERTY(Category = "Custom Movement Component : General Settings", EditAnywhere, BlueprintReadWrite, DisplayName =
        "Can Jump")
    bool bCanJump;

    /** Desired jump height */
    UPROPERTY(Category = "Custom Movement Component : General Settings", EditAnywhere, BlueprintReadWrite)
    float JumpHeight = 300.f;

    /** Maximum acceptable distance for custom pawn capsule/sphere to walk above a surface. */
    //UPROPERTY(Category = "Custom Movement Component : General Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
    //	float GroundHitToleranceDistance;

    /** When sprinting, multiplier applied to Max Walk Speed */
    UPROPERTY(Category = "Custom Movement Component : General Settings", EditAnywhere, BlueprintReadWrite, meta = (
        ClampMin = "0", UIMin = "0"))
    float SpeedBoostMultiplier;

    /**
    * When falling, amount of lateral movement control available to the character.
    * 0 = no control, 1 = full control at max speed of MaxWalkSpeed.
    */
    UPROPERTY(Category = "Custom Movement Component : General Settings", EditAnywhere, BlueprintReadWrite, meta = (
        ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float AirControlRatio;

private:

    bool bIsHit;

    bool bIsInAir;

    FRotator CurrentCapsuleRotation;

    float TimeInAir;

    float LastWalkSpeed;

    bool bIsSprinting;

    bool bIsJumping;

    float SinAngleHitPlane = 0.f;
};
