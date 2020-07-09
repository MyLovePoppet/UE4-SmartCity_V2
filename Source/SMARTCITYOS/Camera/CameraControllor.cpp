// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControllor.h"


FVector CameraControllor::EarthLocation;
Camera* CameraControllor::CameraState;

// Sets default values
CameraControllor::CameraControllor():InputBase()
{
}

/**
 *还有一个ViewProJectionMatrix没实现,此函数放在tick内进行更新
 */
// void CameraControllor::UpdateCameraState()
// {
//     this->CameraState->SetLocation(CameraComponent->GetComponentLocation());
//     this->CameraState->SetRotation(CameraComponent->GetComponentRotation());
//     this->CameraState->SetAspectRatio(CameraComponent->AspectRatio);
//     this->CameraState->SetScreenResolution(GEngine->GameViewport->Viewport->GetSizeXY());
//     this->CameraState->SetFOV(CameraComponent->FieldOfView);
//     this->CameraState->SetIsPerspectView(CameraComponent->ProjectionMode == ECameraProjectionMode::Perspective);
//     this->CameraState->SetUpDirection(CameraComponent->GetUpVector());
//     //this->CameraState->SetViewProjectionMatrix()
// }
