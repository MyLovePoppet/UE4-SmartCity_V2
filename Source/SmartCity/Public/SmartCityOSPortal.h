// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CameraControllorPawn.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmartCityOSPortal.generated.h"

UENUM()
enum class EnumCameraMode :uint8
{
    EnumCameraModeFly,
    EnumCameraModeFlyTo,
    EnumCameraModeGround
};

UCLASS()
class SMARTCITY_API ASmartCityOSPortal : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASmartCityOSPortal();
    //改变模式时的最低高，低于此高度时自动切换度。
    UPROPERTY(Category = "Custom Pawn", EditAnywhere)
    float ChangeModeMinHeight = 100.f;
    UPROPERTY(Category = "Custom Pawn", EditAnywhere)
    AActor* EarthActor;
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    EnumCameraMode GetCameraMode() const { return CameraMode; }
    void SetCameraMode(EnumCameraMode _CameraMode);
    void ChangeCameraMode(EnumCameraMode NewCameraMode);

protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    EnumCameraMode CameraMode;

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    ACameraControllorPawn* CameraControllor;
};

// Fill out your copyright notice in the Description page of Project Settings.
