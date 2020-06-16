// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera.h"
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"

#include "CameraControllorPawn.generated.h"

UCLASS()
class SMARTCITY_API ACameraControllorPawn : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ACameraControllorPawn();

protected:
    TSharedPtr<Camera> CameraState;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    AActor* EarthActor;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float EarthRadius;
    //UPROPERTY(EditAnywhere,BlueprintReadWrite)
    //USpringArmComponent* SpringArm;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    UCameraComponent* CameraComponent;

public:
    void UpdateCameraState();

    Camera* GetCameraState();
    AActor* GetEarthActor();
    float GetEarthRadius();
    //USpringArmComponent* GetSpringArm();
    UCameraComponent* GetCameraComponent();

    void SetCameraState(Camera* _Camera);
    void SetEarthActor(AActor* _EarthActor);
    void SetEarthRadius(float _EarthRadius);
    //void SetSpringArm(USpringArmComponent* _SpringArm);
    void SetCameraComponent(UCameraComponent* _CameraComponent);
};
