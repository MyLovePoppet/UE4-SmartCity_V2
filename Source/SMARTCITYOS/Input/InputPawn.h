// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerInput.h"
#include "InputBase.h"
#include "Camera/CameraComponent.h"
#include "SMARTCITYOS/Camera/CustomGravityPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "SMARTCITYOS/Camera/Camera.h"
#include "InputPawn.generated.h"

UCLASS()
class SMARTCITYOS_API AInputPawn : public APawn
{
    GENERATED_BODY()
public:
    //Camera* CameraState;

    UCameraComponent* UECamera;

public:
    // Sets default values for this pawn's properties    
    AInputPawn();

    static TArray<InputBase*> inputListeners;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    void MouseY(float value);
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    static void RegisterListener(InputBase* lister);
    static void UnRegisterListener(InputBase* lister);

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UCameraComponent* GetUECamera();

    void MouseLMove(float value);
    void ScrollWheel(float value);
    void MouseMMove(float value);
    void MouseRMove(float value);

    void PressedAction(FKey key);
    void ReleasedAction(FKey key);
    void WAxis(float value);
    void DAxis(float value);
    void MouseX(float value);

    /////////////////////////////////////////////////////////////////////////////
    ///Ground模式
    // 
    /////////////////////////////////////////////////////////////////////////////
    // virtual void PostInitializeComponents() override;
    // virtual void Tick(float DeltaSeconds) override;
    virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp,
                           bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                           const FHitResult& Hit) override;
    // End of AActor interface

protected:
    /** Minimum view Pitch, in degrees. */
    //float CameraPitchMin;

    /** Maximum view Pitch, in degrees. */
    //float CameraPitchMax;
public:
    void SetEarthLocation(FVector _EarthLocation) const { MovementComponent->SetEarthLocation(_EarthLocation); }

    /** Handle jump action. */
    void Jump();

    /** Handle sprint action. */
    void Sprint();

    /** Handle stopping sprint action. */
    void StopSprint();

private:
    /** The CapsuleComponent being used for movement collision (by CharacterMovement).*/
	UPROPERTY(EditAnywhere)
    UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere)
		/** Movement component used for movement. */
    UCustomGravityPawnMovement* MovementComponent;
	UPROPERTY(EditAnywhere)

    UCameraComponent* CameraComponent;

public:

    /** Returns CapsuleComponent subobject **/
    FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

    /** Returns SpringArm subobject **/
    //FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }

    /** Returns CustomMovement Component subobject **/
    virtual UPawnMovementComponent* GetMovementComponent() const override;

};
