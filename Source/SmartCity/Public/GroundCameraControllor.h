// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraControllorPawn.h"
#include "CustomGravityPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "GroundCameraControllor.generated.h"

/**
 * 
 */
UCLASS()
class SMARTCITY_API AGroundCameraControllor : public ACameraControllorPawn
{
    GENERATED_BODY()

    AGroundCameraControllor();


    // APawn interface
    virtual void PostInitializeComponents() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp,
                           bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                           const FHitResult& Hit) override;
    // End of AActor interface


    virtual void BeginPlay() override;

    //virtual void UpdateMeshRotation(float DeltaTime);
protected:
    /** Minimum view Pitch, in degrees. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Pawn : Camera Settings")
    float CameraPitchMin;

    /** Maximum view Pitch, in degrees. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Pawn : Camera Settings")
    float CameraPitchMax;
public:
    /** Handle jump action. */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Input", meta = (Keywords = "AddInput"))
    void Jump();

    /** Handle sprint action. */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Input", meta = (Keywords = "AddInput"))
    void Sprint();

    /** Handle stopping sprint action. */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Input", meta = (Keywords = "AddInput"))
    void StopSprint();

    void setPlanetActor(AActor* Planet);

    /** Called to move custom pawn Forward and Backward */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Input", meta = (Keywords = "AddInput"))
    void AddForwardMovementInput(float ScaleValue = 1.0f);

    /** Called to move custom pawn Left and Right */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Input", meta = (Keywords = "AddInput"))
    void AddRightMovementInput(float ScaleValue = 1.0f);

    /**
    * Add input (affecting Pitch) to the SpringArm relative rotation.
    * The added value is the product of UpdateRate & ScaleValue.
    */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Input", meta = (Keywords = "AddInput"))
    void AddCameraPitchInput(float ScaleValue = 0.0f);

    /**
    * Add input (affecting Yaw) to the SpringArm relative rotation.
    * The added value is the product of UpdateRate & ScaleValue.
    */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Input", meta = (Keywords = "AddInput"))
    void AddCameraYawInput(float ScaleValue = 0.0f);

    /** Called to enable CustomPawn & MovementComponent debugging. */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Debuging")
    void EnableDebugging();

    /** Called to disable CustomPawn & MovementComponent debugging. */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn|Debuging")
    void DisableDebugging();

    /**Returns Current Forward Movement Direction. */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn")
    FVector GetCurrentForwardDirection() const;

    /**Returns Current Right Movement Direction. */
    UFUNCTION(BlueprintCallable, Category = "Pawn|CustomPawn")
    FVector GetCurrentRightDirection() const;

    //protected:
    //void SetupPlayerInputComponent(class UInputComponent* InputComponent2) override;

private:

    /** The CapsuleComponent being used for movement collision (by CharacterMovement).*/
    UPROPERTY(Category = "Custom Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* CapsuleComponent;

    /** Movement component used for movement. */
    UPROPERTY(Category = "Custom Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCustomGravityPawnMovement* MovementComponent;
    //下面两个组件声明均为父类中
    /** The camera boom. */
    //UPROPERTY(Category = "Custom Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //class USpringArmComponent* SpringArm;

    /** the main camera associated with this Pawn . */
    //UPROPERTY(Category = "Custom Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //UCameraComponent* Camera;

public:

    /** Returns CapsuleComponent subobject **/
    FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

    /** Returns SpringArm subobject **/
    //FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }

    /** Returns Camera subobject **/
    FORCEINLINE class UCameraComponent* GetCamera() const { return CameraComponent; }

    /** Returns CustomMovement Component subobject **/
    virtual UPawnMovementComponent* GetMovementComponent() const override;
    void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


protected:

    /**Runtime updated values. */

    /** Current Forward Movement Direction*/
    FVector CurrentForwardDirection;

    /** Current Right Movement Direction*/
    FVector CurrentRightDirection;

    float RotationInterpSpeed;

    float MinVelocityToRotateMesh;
};
