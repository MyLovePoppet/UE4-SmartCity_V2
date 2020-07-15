// Fill out your copyright notice in the Description page of Project Settings.
#include "FlyToCameraControllor.h"

FlyToCameraControllor::FlyToCameraControllor()
{
    //CurrentCamera = pSetCamera;
}

void FlyToCameraControllor::FlyTo(Camera TargetCamera, int Tick)
{
    LastCamera = *CameraState;
    FlyToCamera = TargetCamera;
    //接收完后修改回去。

    //更新旋转和位移的目标点和起始点
    StartRot = LastCamera.GetRotation();
    StartLoc = LastCamera.GetLocation() - EarthLocation;
    DesiredRot = FlyToCamera.GetRotation(); //= GetTargetRotation();
    DesiredLoc = FlyToCamera.GetLocation() - EarthLocation;

    //求角度差
    StartArmLength = StartLoc.Size();
    EndArmLength = DesiredLoc.Size();
    //*TargetCamera = *CurrentCamera;
    //从1到TickCount
    TickCount = Tick, FlyProgress = 1;
}

void FlyToCameraControllor::FlyTo(FVector TargetLocation, FRotator TargetRotation, int Tick)
{
    LastCamera = *CameraState;
    //接收完后修改回去。

    //更新旋转和位移的目标点和起始点
    StartRot = LastCamera.GetRotation();
    StartLoc = LastCamera.GetLocation() - EarthLocation;
    DesiredRot = TargetRotation; //= GetTargetRotation();
    DesiredLoc = TargetLocation - EarthLocation;

    //求角度差
    StartArmLength = StartLoc.Size();
    EndArmLength = DesiredLoc.Size();
    //*TargetCamera = *CurrentCamera;
    //从1到TickCount
    TickCount = Tick, FlyProgress = 1;
}

void FlyToCameraControllor::OnKeyDown(FKey key)
{
    if (key == EKeys::SpaceBar)
    {
        Camera sz;
        sz.SetLocation(FVector(-384261.25, -2430907, 2337320.25));
        sz.SetRotation(FRotator(-29.92, 70.27, 0));
        FlyTo(sz, 90);
    }
}

void FlyToCameraControllor::OnTick(float DeltaTime)
{
    if (FlyProgress <= TickCount)
    {
        UpdateDesiredArmLocation(DeltaTime);
        FlyProgress++;
    }
}


void FlyToCameraControllor::UpdateDesiredArmLocation(float DeltaTime)
{
    // //帧率越低，单位时间的偏移越小。
    // const FRotator RotTemp = FRotator(FMath::QInterpTo(FQuat(StartRot), FQuat(DesiredRot),
    //                                                    FlyProgress * 1.f / TickCount,
    //                                                    1.f));
    // const FVector LocTemp = FMath::VInterpTo(StartLoc, DesiredLoc, FlyProgress * 1.f / TickCount, 1.f);
    // // GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::SanitizeFloat(FlyProgress * 1.f / TickCount));
    // // GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Blue, DesiredLoc.ToString());

    //将Location转为Rotation的插值，实现球面移动。
    const FRotator LocTemp = FRotator(FMath::QInterpTo(FQuat(StartLoc.Rotation()), FQuat(DesiredLoc.Rotation()),
                                                       FlyProgress * 1.f / TickCount,
                                                       1.f));
    //臂长插值
    float ArmLengthTemp = FMath::FInterpTo(StartArmLength, EndArmLength, FlyProgress * 1.f / TickCount, 1.f);
    //球面插值转回Location
    FVector TargetLocation = LocTemp.Vector() * ArmLengthTemp;

    //相机视角旋转插值
    const FRotator RotTemp = FRotator(FMath::QInterpTo(FQuat(StartRot), FQuat(DesiredRot),
                                                       FlyProgress * 1.f / TickCount,
                                                       1.f));
    //应用到当前的CameraState
    CameraState->SetRotation(RotTemp);
    CameraState->SetLocation(TargetLocation + EarthLocation);
}
