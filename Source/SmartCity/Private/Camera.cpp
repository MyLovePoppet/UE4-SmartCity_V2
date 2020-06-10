// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

float Camera::GetFOV() const
{
    return FOV;
}

FVector Camera::GetLocation() const
{
    return Location;
}

FRotator Camera::GetRotation() const
{
    return Rotation;
}

FVector Camera::GetUpDirection() const
{
    return UpDirection;
}

float Camera::GetAspectRatio() const
{
    return AspectRatio;
}

FVector2D Camera::GetScreenResolution() const
{
    return ScreenResolution;
}

bool Camera::GetIsPerspectView() const
{
    return IsPerspectView;
}

FMatrix Camera::GetViewProjectionMatrix() const
{
    return ViewProjectionMatrix;
}

EnumCameraMode Camera::GetCameraMode() const
{
    return CameraMode;
}

void Camera::SetFOV(float _FOV)
{
    this->FOV = _FOV;
}

void Camera::SetLocation(FVector _Location)
{
    this->Location = _Location;
}

void Camera::SetRotation(FRotator _Rotation)
{
    this->Rotation = _Rotation;
}

void Camera::SetUpDirection(FVector _UpDirection)
{
    this->UpDirection = _UpDirection;
    //
}

void Camera::SetAspectRatio(float _AspectRatio)
{
    this->AspectRatio = _AspectRatio;
}

void Camera::SetScreenResolution(FVector2D _ScreenResolution)
{
    this->ScreenResolution = _ScreenResolution;
}

void Camera::SetIsPerspectView(bool _IsPerspectView)
{
    this->IsPerspectView = _IsPerspectView;
}

void Camera::SetViewProjectionMatrix(FMatrix _ViewProjectionMatrix)
{
    this->ViewProjectionMatrix = _ViewProjectionMatrix;
}

void Camera::SetCameraMode(EnumCameraMode _CameraMode)
{
    this->CameraMode = _CameraMode;
}
