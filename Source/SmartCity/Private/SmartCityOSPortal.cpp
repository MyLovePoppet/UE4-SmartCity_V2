// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartCityOSPortal.h"

// Sets default values
ASmartCityOSPortal::ASmartCityOSPortal()
{
    PrimaryActorTick.bCanEverTick = true;
    /*
    switch (CameraMode)
    {
    case EnumCameraMode::EnumCameraModeFly:
    CameraControllor = CreateDefaultSubobject<AFlyModeCameraControllor>(TEXT("FlyModeCamera"));
    break;
    case EnumCameraMode::EnumCameraModeGround:
    CameraControllor = CreateDefaultSubobject<AGroundCameraControllor>(TEXT("GroundModeCamera"));
    break;
    case EnumCameraMode::EnumCameraModeFlyTo:
    CameraControllor = CreateDefaultSubobject<AFlyToCameraControllor>(TEXT("FlyToModeCamera"));
    }
    */
}

// Called when the game starts or when spawned
void ASmartCityOSPortal::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ASmartCityOSPortal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
