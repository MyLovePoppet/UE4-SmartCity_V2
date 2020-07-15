// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraControllor.h"

/**
 * 
 */
class SMARTCITYOS_API FlyToCameraControllor : public CameraControllor
{
    //保存上一次相机的状态
    Camera LastCamera;
    //目标相机状态
    Camera FlyToCamera;
    //当前相机状态
    //在父类的静态变量CameraState
    //Camera* CurrentCamera;
    //飞行的进度，小于TickCount
    int FlyProgress = INT_MAX;
    //飞行总Tick
    int TickCount = 0;
    //起始点和终点的旋转和位置
    FRotator DesiredRot;
    FRotator StartRot;
    FVector DesiredLoc;
    FVector StartLoc;

    float StartArmLength;
    float EndArmLength;
public:
    FlyToCameraControllor();
    // {
    // }

    void FlyTo(Camera TargetCamera, int tick);
    void FlyTo(FVector TargetLocation, FRotator TargetRotation, int Tick);

    // {
    //     lastCamera = *pSetCamera;;
    //     flyToCamera = camera;
    //     tickCount = tick;
    //     flyprogress = 0;
    // }
    virtual void OnKeyDown(FKey key) override;
    virtual void OnTick(float DeltaTime) override;
    // {
    //     flyprogress++;
    //     currentCamera;
    //     pSetCamera->SetCameraState(currentCamera);
    // }
private:
    //每个tick的偏移量是相同的。
    //DeltaTIme并没有参与运算，因此每次的偏移值速度是不定的，帧率越高速度越快
    void UpdateDesiredArmLocation(float DeltaTime);
};
