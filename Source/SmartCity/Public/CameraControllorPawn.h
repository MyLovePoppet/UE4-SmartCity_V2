// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera.h"
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"

#include "CameraControllorPawn.generated.h"

struct DepthPixel //定义深度像素结构体
{
    float depth;
    char stencil;
    char unused1;
    char unused2;
    char unused3;
};

UCLASS()
class SMARTCITY_API ACameraControllorPawn : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ACameraControllorPawn();
    void Tick(float DeltaTime) override;
    void Init(AActor* EarthActor);
protected:
    TSharedPtr<Camera> CameraState;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    AActor* EarthActor;
public:
    AActor* GetEarthActor() const { return EarthActor; }

    void SetEarthActor(AActor* _EarthActor);

protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float EarthRadius;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    UCameraComponent* CameraComponent;

    void UpdateCameraState();
    bool ViewPortToWorldSafety(FVector2D ScreenPos, FVector& out_WorldPos);


    Camera* GetCameraState();
    AActor* GetEarthActor();
    float GetEarthRadius();
    UCameraComponent* GetCameraComponent();

    void SetCameraState(Camera* _Camera);
    void SetEarthRadius(float _EarthRadius);
    void SetCameraComponent(UCameraComponent* _CameraComponent);

private:
    bool ViewPortToWorld(FVector2D ScreenPos, FVector& outWorldPos);
    float DepthOnScreen(FVector2D TargetPoint);
    TArray<DepthPixel> GetDepthSurface(FVector2D& OutRenderTargetSize);
};
