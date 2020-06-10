// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EnumCameraMode : uint8;

/**
 * 
 */
class SMARTCITY_API Camera
{
public:
    Camera();
    ~Camera();

private:
    float FOV;
    FVector Location;
    FRotator Rotation;
    FVector UpDirection;
    float AspectRatio;
    FVector2D ScreenResolution;
    bool IsPerspectView;
    FMatrix ViewProjectionMatrix;
    EnumCameraMode CameraMode;


public:
    float GetFOV() const;
    FVector GetLocation() const;
    FRotator GetRotation() const;
    FVector GetUpDirection() const;
    float GetAspectRatio() const;
    FVector2D GetScreenResolution() const;
    bool GetIsPerspectView() const;
    FMatrix GetViewProjectionMatrix() const;
    EnumCameraMode GetCameraMode() const;

public:
    void SetFOV(float _FOV);
    void SetLocation(FVector _Location);
    void SetRotation(FRotator _Rotation);
    void SetUpDirection(FVector _UpDirection);
    void SetAspectRatio(float _AspectRatio);
    void SetScreenResolution(FVector2D _ScreenResolution);
    void SetIsPerspectView(bool _IsPerspectView);
    void SetViewProjectionMatrix(FMatrix _ViewProjectionMatrix);
    void SetCameraMode(EnumCameraMode _CameraState);
};
