// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"

#include "CameraControllorPawn.generated.h"

UCLASS()
class SMARTCITY_API ACameraControllorPawn : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ACameraControllorPawn();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    AActor* EarthActor;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float EarthRadius;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    USpringArmComponent* SpringArm;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    UCameraComponent* CameraComponent;

public:
    virtual void OnLeftMouseUp();
    virtual void OnLeftMouseDown();
    virtual void OnRightMouseUp();
    virtual void OnRightMouseDown();
    virtual void OnMidMouseUp();
    virtual void OnMidMouseDown();
    virtual void OnMouseMove(float Axis);
    virtual void OnScrollWheel(float Axis);
};
