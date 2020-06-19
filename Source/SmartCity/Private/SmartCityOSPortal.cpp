// Fill out your copyright notice in the Description page of Project Settings.
#include "SmartCityOSPortal.h"
#include "FlyModeCameraControllor.h"
#include "GroundCameraControllor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASmartCityOSPortal::ASmartCityOSPortal()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASmartCityOSPortal::BeginPlay()
{
    Super::BeginPlay();
    ChangeCameraMode(this->CameraMode);
}
//改变相机模式
void ASmartCityOSPortal::SetCameraMode(EnumCameraMode NewCameraMode)
{
    if (NewCameraMode == this->CameraMode)
        return;
    ChangeCameraMode(NewCameraMode);
}

//不检查当前模式与新模式是否相同，强制改变
void ASmartCityOSPortal::ChangeCameraMode(EnumCameraMode NewCameraMode)
{
    const FRotator SpawnRotation = FRotator::ZeroRotator;
    const FVector SpawnLocation = CameraControllor ? CameraControllor->GetActorLocation() : FVector::ZeroVector;

    FTransform Transform;
    Transform.SetLocation(SpawnLocation);
    Transform.SetRotation(FQuat(SpawnRotation));

    //Set Spawn Collision Handling Override 
    FActorSpawnParameters ActorSpawnParams;
    ActorSpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ACameraControllorPawn* NewControllor;

    switch (NewCameraMode)
    {
    case EnumCameraMode::EnumCameraModeGround:
        {
            NewControllor = Cast<AGroundCameraControllor>(
                UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AGroundCameraControllor::StaticClass(),
                                                                   Transform));
            break;
        }
    case EnumCameraMode::EnumCameraModeFly:
        {
            NewControllor = Cast<AFlyModeCameraControllor>(
                UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AFlyModeCameraControllor::StaticClass(),
                                                                   Transform));
            break;
        }
        // case EnumCameraMode::EnumCameraModeFlyTo: break;
        default:
            return;
    }
    if (NewControllor != nullptr)
    {
        if (CameraControllor != nullptr)
        {
            CameraControllor->Destroy();
        }
        NewControllor->Init(EarthActor);
        UGameplayStatics::FinishSpawningActor(NewControllor, Transform);

        CameraControllor = NewControllor;
        this->CameraMode = NewCameraMode;
    }
}

// Called every frame
void ASmartCityOSPortal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (CameraMode == EnumCameraMode::EnumCameraModeFly)
    {
        float CurrentHeight = static_cast<AFlyModeCameraControllor*>(CameraControllor)->GetHeight();
        if (CurrentHeight < ChangeModeMinHeight)
        {
            GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red,TEXT("Changing Mode!"));
            SetCameraMode(EnumCameraMode::EnumCameraModeGround);
        }
    }
}
