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

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    EnumCameraMode CameraMode;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    ACameraControllorPawn* CameraControllor;
};
// Fill out your copyright notice in the Description page of Project Settings.
