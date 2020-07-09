// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"
/*#include "Kismet/GameplayStatics.h"*/
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

FRotator Camera::GetRelativeRotation() const 
{ 
    return RelativeRotation; 
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

FIntPoint Camera::GetScreenResolutionIntPoint() const
{
	FIntPoint ScreenResolutionIntPoint = FIntPoint((int)(ScreenResolution.X), (int)(ScreenResolution.Y));
	return ScreenResolutionIntPoint;
}

bool Camera::GetIsPerspectView() const
{
    return IsPerspectView;
}

FMatrix Camera::GetViewProjectionMatrix() const
{
    return ViewProjectionMatrix;
}

FConvexVolume Camera::GetConvexVolume() const
{
	FConvexVolume convexVolume;
	GetViewFrustumBounds(convexVolume, ViewProjectionMatrix, false);
	//GetViewFrustumBounds(convexVolume, GUESceneTools.GetViewProjectionMatrix(), false);	
	return convexVolume;
}

float Camera::GetCameraHeight()
{
	float CameraHeight = FVector::Distance(this->GetLocation(), EarthLocation) - 6378137.0;
	return CameraHeight;
}

float Camera::GetNearClipPlane()
{
	return this->NearClipPlane;
}

FVector Camera::GetCameraRight() const
{
    return CameraRight;
}

FVector Camera::GetCameraForward() const
{
    return CameraForward;
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

void Camera::SetRotation(FQuat _Rotation)
{
	this->Rotation = _Rotation.Rotator();
}

void Camera::SetRelativeRotation(FRotator _RelativeRotation) 
{ 
    RelativeRotation=_RelativeRotation; 
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

void Camera::SetNearClipPlane(float _NearClipPlane)
{
	//GEngine->Exec(GWorld, TEXT("r.SetNearClipPlane 100.0"), *GLog);
	this->NearClipPlane = _NearClipPlane;
}

void Camera::SetCameraRight(const FVector& _CameraRight)
{
    this->CameraRight = _CameraRight;
}

void Camera::SetCameraForward(const FVector& _CameraForward)
{
    this->CameraForward = CameraForward;
}

void Camera::SetEarthLocation(FVector _EarthLocation)
{
	this->EarthLocation = _EarthLocation;
}
