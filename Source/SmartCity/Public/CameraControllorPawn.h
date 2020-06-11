// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera.h"
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"

#include "CameraControllorPawn.generated.h"

UCLASS()
class SMARTCITY_API ACameraControllorPawn : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ACameraControllorPawn();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
    TSharedPtr<Camera> CameraState;

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    AActor* EarthActor;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float EarthRadius;
    //UPROPERTY(EditAnywhere,BlueprintReadWrite)
    //USpringArmComponent* SpringArm;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    UCameraComponent* CameraComponent;
public:
    virtual void UpdateCameraState();
    
    //接收鼠标键盘事件函数
    void OnMouseUp();
    void OnMouseDown();
    void OnKeyUp();
    void OnKeyDown();
    void OnKeyMove(float Axis);
    //真正的事件处理函数
    virtual void OnMouseUpAction(FKey Key);
    virtual void OnMouseDownAction(FKey Key);
    virtual void OnMouseXMove(float Axis);
    virtual void OnMouseYMove(float Axis);
    virtual void OnScrollWheel(float Axis);
    virtual void OnKeyUpAction(FKey Key);
    virtual void OnKeyDownAction(FKey Key);
    virtual void OnKeyAxisMove(FKey Key, float Axis);


    Camera* GetCameraState();
    AActor* GetEarthActor();
    float GetEarthRadius();
    //USpringArmComponent* GetSpringArm();
    UCameraComponent* GetCameraComponent();

    void SetCameraState(Camera* _Camera);
    void SetEarthActor(AActor* _EarthActor);
    void SetEarthRadius(float _EarthRadius);
    //void SetSpringArm(USpringArmComponent* _SpringArm);
    void SetCameraComponent(UCameraComponent* _CameraComponent);
};
