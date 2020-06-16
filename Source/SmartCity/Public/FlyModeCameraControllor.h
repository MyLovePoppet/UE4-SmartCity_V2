// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraControllorPawn.h"

#include "FlyModeCameraControllor.generated.h"
class AFlyModeCameraControllor;

/**
 * 
 */
UCLASS()
class SMARTCITY_API AFlyModeCameraControllor : public ACameraControllorPawn
{
    GENERATED_BODY()

public:
    bool LineSphereFirstIntersect(FVector lineOri, FVector lineDir, FVector sphereOri, float radius,
                                  FVector& intersectPt);
    // Sets default values for this pawn's properties
    AFlyModeCameraControllor();


    //后续缩放比例
    float NowScale = 1.f;
    //初始缩放，后续的缩放都会乘以该缩放。
    UPROPERTY(Category = "Custom Pawn", EditAnywhere)
    float InitScale = 0.0001f;
    //使DebugLine的粗细随Scale变化。
    float Thickness = 10;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //原始缩放为1时，地球的大小
    UPROPERTY(Category = "Custom Pawn", EditAnywhere)
    float OriginRadius = 6378137.f;
    UPROPERTY(Category = "Custom Pawn", EditAnywhere)
    float ArmLength;

    bool ScreenCursorInfoToWorld(const FVector2D screenCursorPt, FVector& WorldPt, FVector& WorldDir);

    void CalcDragRotation(const FVector2D inCursorPt, const FVector2D inNextCursorPt);

    bool CursorPointOnEarth(FVector2D CursorPoint, FVector& intersectPt);

    FVector GetPointToCenterVector(FVector TouchPoint);

    void RotateEarthByAxis(float AngleDeg);
    void ScaleBall(float Sign = 1.f);

    FVector GetHorizontalVector();

    void Zoom(float Speed);
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void LeftButtonDown();
    virtual void LeftButtonUp();
    virtual void LeftButtonHold(float val);

    virtual void MidButtonDown();
    virtual void MidButtonUp();
    virtual void MidButtonHold(float val);

    virtual void RightButtonDown();
    virtual void RightButtonHold(float val);

    virtual void OnScrollWheelUpPress(float val);
private:
    FVector2D oldCursorPt;
    FVector2D currentCursorPt;

    //中间旋转所绕轴
    FVector MidHoldAxis;
    FVector OldLocationOnEarth;
};
