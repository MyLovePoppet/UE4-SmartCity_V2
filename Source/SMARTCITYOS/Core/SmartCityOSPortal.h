// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Synchronize.h"
#include "..\Input\InputPawn.h"
#include "..\Camera\CameraControllor.h"
#include "..\Camera\FlyModeCameraControllor.h"
#include "..\Camera\GroundModeCameraControllor.h"
#include "SmartCityOSPortal.generated.h"


UCLASS()
class SMARTCITYOS_API ASmartCityOSPortal : public AActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    AActor *EarthActor;
    //定义Camera对象(SCOS)
    Camera* CameraState;
    Camera LastCameraState;
    CameraControllor* SCOSCameraControllor;
    FlyModeCameraControllor* FlyModeCache;
    GroundModeCameraControllor* GroundModeCache;
    //将EarthLocation设置深圳
    FVector EarthLocation = FVector(0.f,0.f,0.f);

    //定义UE相机
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* UECamera;
    //UPROPERTY(VisibleAnywhere)
    AInputPawn* UserInput;

    //AActor *UdpReceiverActor;

public:
    // Sets default values for this actor's properties
    ASmartCityOSPortal();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    //virtual void PostInitializeComponents() override;
    virtual void Tick(float DeltaTime) override;

private:

    void GenerateCamera();

    //判断CameraState是否发生改变
    bool IsCameraStateChanged(const Camera& LastCameraState, const Camera& NewCameraState);


    //改变相机模式
    void ChangeControllorMod();
};
