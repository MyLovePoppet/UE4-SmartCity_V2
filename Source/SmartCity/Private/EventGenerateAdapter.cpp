// Fill out your copyright notice in the Description page of Project Settings.


#include "EventGenerateAdapter.h"

#include "FlyModeCameraControllor.h"
#include "FlyToCameraControllor.h"
#include "GroundCameraControllor.h"

// Sets default values
AEventGenerateAdapter::AEventGenerateAdapter()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
void AEventGenerateAdapter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AEventGenerateAdapter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if ((FVector::Dist(CameraControllor->GetCameraComponent()->GetComponentLocation(),
                       CameraControllor->GetEarthActor()->GetActorLocation()) - CameraControllor->GetEarthRadius())
        / CameraControllor->GetEarthRadius() < 0.01)
    {
        GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, "Can change mode to Ground!");
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, "Current Camera mode: Fly mode.");
    }
}
