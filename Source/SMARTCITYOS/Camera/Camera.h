// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */
class SMARTCITYOS_API Camera
{
public:
    Camera();
    ~Camera();

private:
    float FOV;
    FVector Location;
    FRotator Rotation;
	FRotator RelativeRotation;
    FVector UpDirection;
    float AspectRatio;
    FVector2D ScreenResolution;
    bool IsPerspectView;
    FMatrix ViewProjectionMatrix;
	float NearClipPlane;
	FVector CameraRight;

	FVector EarthLocation;

public:
    float GetFOV() const;
    FVector GetLocation() const;
	FRotator GetRotation() const;
	FRotator GetRelativeRotation() const;
    FVector GetUpDirection() const;
    float GetAspectRatio() const;
    FVector2D GetScreenResolution() const;
    bool GetIsPerspectView() const;
    FMatrix GetViewProjectionMatrix() const;
	FConvexVolume GetConvexVolume() const; // 视锥体
	float GetCameraHeight();
	float GetNearClipPlane();
	FVector GetCameraRight() const;

	FIntPoint GetScreenResolutionIntPoint() const;

public:
    void SetFOV(float _FOV);
    void SetLocation(FVector _Location);
	void SetRotation(FRotator _Rotation);
	void SetRelativeRotation(FRotator _RelativeRotation);
	void SetRotation(FQuat _Rotation);
    void SetUpDirection(FVector _UpDirection);
    void SetAspectRatio(float _AspectRatio);
    void SetScreenResolution(FVector2D _ScreenResolution);
    void SetIsPerspectView(bool _IsPerspectView);
    void SetViewProjectionMatrix(FMatrix _ViewProjectionMatrix);
	void SetNearClipPlane(float _NearClipPlane);

	void SetCameraRight(const FVector& _CameraRight);

	void SetEarthLocation(FVector _EarthLocation);

	Camera* CameraTransform(FMatrix InMatrix);
};
