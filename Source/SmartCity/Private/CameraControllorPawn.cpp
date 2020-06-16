// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControllorPawn.h"

#include "GameFramework/PlayerInput.h"

// Sets default values
ACameraControllorPawn::ACameraControllorPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCamera"));

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    CameraState = MakeShareable<Camera>(new Camera());
}

/**
 *还有一个ViewProJectionMatrix没实现,此函数放在tick内进行更新
 */
void ACameraControllorPawn::UpdateCameraState()
{
    this->CameraState->SetLocation(CameraComponent->GetComponentLocation());
    this->CameraState->SetRotation(CameraComponent->GetComponentRotation());
    this->CameraState->SetAspectRatio(CameraComponent->AspectRatio);
    this->CameraState->SetScreenResolution(GEngine->GameViewport->Viewport->GetSizeXY());
    this->CameraState->SetFOV(CameraComponent->FieldOfView);
    this->CameraState->SetIsPerspectView(CameraComponent->ProjectionMode == ECameraProjectionMode::Perspective);
    this->CameraState->SetUpDirection(CameraComponent->GetUpVector());
    //this->CameraState->SetViewProjectionMatrix()
}

Camera* ACameraControllorPawn::GetCameraState()
{
    return CameraState.Get();
}

AActor* ACameraControllorPawn::GetEarthActor()
{
    return EarthActor;
}

float ACameraControllorPawn::GetEarthRadius()
{
    return EarthRadius;
}


UCameraComponent* ACameraControllorPawn::GetCameraComponent()
{
    return CameraComponent;
}

void ACameraControllorPawn::SetCameraState(Camera* _Camera)
{
    this->CameraState = MakeShareable(_Camera);
}

void ACameraControllorPawn::SetEarthActor(AActor* _EarthActor)
{
    this->EarthActor = _EarthActor;
}

void ACameraControllorPawn::SetEarthRadius(float _EarthRadius)
{
    this->EarthRadius = _EarthRadius;
}

void ACameraControllorPawn::SetCameraComponent(UCameraComponent* _CameraComponent)
{
    this->CameraComponent = _CameraComponent;
}
